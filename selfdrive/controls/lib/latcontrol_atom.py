import math
import numpy as np

from cereal import log
from common.realtime import DT_CTRL
from common.numpy_fast import clip, interp

from common.params import Params
from decimal import Decimal


from selfdrive.controls.lib.latcontrol import LatControl, MIN_STEER_SPEED
from selfdrive.controls.lib.pid import PIDController

from selfdrive.controls.lib.latcontrol_torque import LatControlTorque
from selfdrive.controls.lib.latcontrol_lqr import LatControlLQR




class LatCtrlToqATOM(LatControlTorque):
  def __init__(self, CP, CI, TORQUE):
    self.mpc_frame = 0
    self.params = Params()
    self.sat_count_rate = 1.0 * DT_CTRL
    self.sat_limit = CP.steerLimitTimer
    self.sat_count = 0. 
    
    # we define the steer torque scale as [-1.0...1.0] 
    self.steer_max = 1.0

    self.pid = PIDController(TORQUE.kp, TORQUE.ki,
                             k_f=TORQUE.kf, pos_limit=self.steer_max, neg_limit=-self.steer_max)
    self.get_steer_feedforward = CI.get_steer_feedforward_function()
    self.use_steering_angle = TORQUE.useSteeringAngle
    self.friction = TORQUE.friction
    self.kf = TORQUE.kf

    self.live_tune_enabled = False
    self.lt_timer = 0



class LatCtrlLqrATOM(LatControlLQR):
  def __init__(self, CP, CI, LQR):
    self.mpc_frame = 0
    self.live_tune_enabled = False
    self.params = Params()
    self.sat_count_rate = 1.0 * DT_CTRL 
    self.sat_limit = CP.steerLimitTimer 
    self.sat_count = 0. 
    
    # we define the steer torque scale as [-1.0...1.0]
    self.steer_max = 1.0
    self.ll_timer = 0



    self.scale = LQR.scale
    self.ki = LQR.ki

    self.A = np.array(LQR.a).reshape((2, 2))
    self.B = np.array(LQR.b).reshape((2, 1))
    self.C = np.array(LQR.c).reshape((1, 2))
    self.K = np.array(LQR.k).reshape((1, 2))
    self.L = np.array(LQR.l).reshape((2, 1))
    self.dc_gain = LQR.dcGain

    self.x_hat = np.array([[0], [0]])
    self.i_unwind_rate = 0.3 * DT_CTRL
    self.i_rate = 1.0 * DT_CTRL

    self.reset()





class LatControlATOM(LatControl):
  def __init__(self, CP, CI):
    super().__init__(CP, CI)


    self.torqueMaxSpeed =  CP.torqueMaxSpeed
    self.lqr = CP.lateralTuning.atom.lqr
    self.torque = CP.lateralTuning.atom.torque

    self.LaLqr = LatCtrlLqrATOM( CP, CI, self.lqr )
    self.LaToq = LatCtrlToqATOM( CP, CI, self.torque )

    self.output_torque = 0
    self.reset()

  def reset(self):
    super().reset()
    self.LaLqr.reset()
    self.LaToq.reset()

  def update(self, active, CS, CP, VM, params, last_actuators, desired_curvature, desired_curvature_rate, llk):
    atom_log = log.ControlsState.LateralATOMState.new_message()

    if CS.vEgo < MIN_STEER_SPEED or not active:
      output_torque = 0.0
      lqr_desired_angle = 0.
      atom_log.active = False
      if not active:
        self.reset()
    else:
      lqr_output_torque, lqr_desired_angle, lqr_log  = self.LaLqr.update( active, CS, CP, VM, params, last_actuators, desired_curvature, desired_curvature_rate, llk )
      toq_output_torque, toq_desired_angle, toq_log  = self.LaToq.update( active, CS, CP, VM, params, last_actuators, desired_curvature, desired_curvature_rate, llk )

      if CS.vEgo < self.torqueMaxSpeed:  # 12.5 45 kph
        selected = 1.0  # toq
      else:
        #output_torque = lqr_output_torque
        lqr_delta = lqr_output_torque - self.output_torque
        toq_delta = toq_output_torque - self.output_torque

        # 1. 전과 비교하여 변화량이 적은 부분 선택.
        abs_lqr = abs( lqr_delta ) 
        abs_toq = abs( toq_delta ) 
        if abs_lqr > abs_toq:
          selected = 1.0   # toq
        else: 
          selected = -1.0  # lqr
          
      output_torque = interp( selected, [-1, 1], [lqr_output_torque, toq_output_torque] )
      output_torque = clip( output_torque, -self.steer_max, self.steer_max )

      # 2. log
      atom_log.active = True    
      atom_log.steeringAngleDeg = lqr_log.steeringAngleDeg
      atom_log.i = lqr_log.i
      atom_log.saturated = lqr_log.saturated      
      atom_log.lqrOutput = lqr_log.lqrOutput
      atom_log.output = output_torque      

      atom_log.p1 = toq_log.p
      atom_log.i1 = toq_log.i
      atom_log.d1 = toq_log.d
      atom_log.f1 = toq_log.f
      atom_log.selected = selected
    
    self.output_torque = output_torque
    desired_angle = lqr_desired_angle

    return output_torque, desired_angle, atom_log
