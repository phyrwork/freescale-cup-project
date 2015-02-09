function gui = DemoLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LinescanView, 'linescan0', 'figure', fig, 'position', [2,5,1]);
    gui = gui.addView(@TrackPositionView, 'TrackPosition', 'figure', fig, 'position', [2,5,6]);
    gui = gui.addView(@WheelSpeedView, 'w_fl', 'figure', fig, 'position', [2,5,2]);
    gui = gui.addView(@WheelSpeedView, 'w_fr', 'figure', fig, 'position', [2,5,7]);
    gui = gui.addView(@WheelSpeedView, 'w_rl', 'target', 'w*_rl', 'figure', fig, 'position', [2,5,3]);
    gui = gui.addView(@WheelSpeedView, 'w_rr', 'target', 'w*_rr', 'figure', fig, 'position', [2,5,8]);
    gui = gui.addView(@MotorTorqueView, 'T_rl', 'target', 'T*_rl', 'figure', fig, 'position', [2,5,4]);
    gui = gui.addView(@MotorTorqueView, 'T_rr', 'target', 'T*_rr', 'figure', fig, 'position', [2,5,9]);
    gui = gui.addView(@MotorCurrentView, 'i_rl', 'figure', fig, 'position', [2,5,5]);
    gui = gui.addView(@MotorCurrentView, 'i_rr', 'figure', fig, 'position', [2,5,10]);
end

