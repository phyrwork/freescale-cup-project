function gui = MotorTorqueControlLayout( gui )
%MotorTorqueControlLayout Load a simple GUI window showing car vision and motor
%load current.

    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@WheelSpeedView, 'w_v', 'target', 'w*_v', 'figure', fig, 'position', [2,3,1]);
    %gui = gui.addView(@WheelSpeedView, 'w_rr', 'figure', fig, 'position', [2,3,4]);
    gui = gui.addView(@MotorTorqueView, 'T_rl', 'target', 'T*_rl', 'figure', fig, 'position', [2,3,2]);
    gui = gui.addView(@MotorTorqueView, 'T_rr', 'target', 'T*_rl','figure', fig, 'position', [2,3,5]);
    gui = gui.addView(@MotorDutyView, 'D_rl', 'figure', fig, 'position', [2,3,3]);
    gui = gui.addView(@MotorDutyView, 'D_rr', 'figure', fig, 'position', [2,3,6]);
end