%% Parameters for Battery State-of-Charge Estimation
% 
% This example shows how to estimate the battery state-of-charge (SOC) by 
% using a Kalman filter. The initial SOC of the battery is equal to 0.5. 
% The estimator uses an initial condition for the SOC equal to 0.8. The 
% battery keeps charging and discharging for 6 hours. The extended Kalman 
% filter estimator converges to the real value of the SOC in less than 10 
% minutes and then follows the real SOC value. To use a different Kalman 
% filter implementation, in the SOC Estimator (Kalman Filter) block, set 
% the Filter type parameter to the desired value.

% Copyright 2022 The MathWorks, Inc.

%% System Parameters
SOC_vec = [0, .1, .25, .5, .75, .9, 1]; % Vector of state-of-charge values, SOC
T_vec   = [278, 293, 313];              % Vector of temperatures, T, (K)
AH      = 27;                           % Cell capacity, AH, (A*hr) 
thermal_mass = 100;                     % Thermal mass (J/K)
initialSOC = 0.5;                       % Battery initial SOC
V0_mat  = [3.49, 3.5, 3.51; 3.55, 3.57, 3.56; 3.62, 3.63, 3.64;...
    3.71, 3.71, 3.72; 3.91, 3.93, 3.94; 4.07, 4.08, 4.08;...
    4.19, 4.19, 4.19];                          % Open-circuit voltage, V0(SOC,T), (V)
R0_mat  = [.0117, .0085, .009; .011, .0085, .009;...
    .0114, .0087, .0092; .0107, .0082, .0088; .0107, .0083, .0091;...
    .0113, .0085, .0089; .0116, .0085, .0089];  % Terminal resistance, R0(SOC,T), (ohm)

R1_mat  = [.0109, .0029, .0013; .0069, .0024, .0012;...
    .0047, .0026, .0013; .0034, .0016, .001; .0033, .0023, .0014;...
    .0033, .0018, .0011; .0028, .0017, .0011];  % First polarization resistance, R1(SOC,T), (ohm)
tau1_mat = [20, 36, 39; 31, 45, 39; 109, 105, 61;...
    36, 29, 26; 59, 77, 67; 40, 33, 29; 25, 39, 33]; % First time constant, tau1(SOC,T), (s)

cell_area = 0.1019; % Cell area (m^2)
h_conv    = 5;      % Heat transfer coefficient (W/(K*m^2))

%% Kalman Filter
Q    = [1e-4 0; 0 1e-4]; % Covariance of the process noise, Q
R    = 0.7;              % Covariance of the measurement noise, R
P0   = [1e-5 0; 0 1];    % Initial state error covariance, P0
SOC0 = 0.8;              % Initial SOC for estimator 
Ts   = 1;                % Sample time