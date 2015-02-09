function gui = DemoLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LinescanView, 'linescan0', 'figure', fig, 'position', [2,4,1]);
    gui = gui.addView(@TrackPositionView, 'TrackPosition', 'figure', fig, 'position', [2,4,5]);
    gui = gui.addView(@WheelSpeedView, 'w_rl', 'target', 'w*_rl', 'figure', fig, 'position', [2,4,2]);
    gui = gui.addView(@WheelSpeedView, 'w_rr', 'target', 'w*_rr', 'figure', fig, 'position', [2,4,6]);
    gui = gui.addView(@MotorTorqueView, 'T_rl', 'target', 'T*_rl', 'figure', fig, 'position', [2,4,3]);
    gui = gui.addView(@MotorTorqueView, 'T_rr', 'target', 'T*_rr', 'figure', fig, 'position', [2,4,7]);
    gui = gui.addView(@MotorCurrentView, 'i_rl', 'figure', fig, 'position', [2,4,4]);
    gui = gui.addView(@MotorCurrentView, 'i_rr', 'figure', fig, 'position', [2,4,8]);
end

