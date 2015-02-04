function gui = MotorSpeedControlLayout( gui )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.

    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@WheelSpeedView, 'w_rl', 'target', 'w*_rl', 'figure', fig, 'position', [2,3,1]);
    gui = gui.addView(@WheelSpeedView, 'w_rr', 'target', 'w*_rr', 'figure', fig, 'position', [2,3,4]);
    gui = gui.addView(@MotorDutyView,  'D_rl', 'figure', fig, 'position', [2,3,2]);
    gui = gui.addView(@MotorDutyView,  'D_rr', 'figure', fig, 'position', [2,3,5]);
    gui = gui.addView(@WheelSlipView,  's_rl', 'figure', fig, 'position', [2,3,3]);
    %gui = gui.addView(@WheelSlipView, 's_rr', 'figure', fig, 'position', [2,3,6]);
end

