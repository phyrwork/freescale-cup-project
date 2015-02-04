function gui = VisionCurrentLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LinescanView, 'linescan0', 'figure', fig, 'position', [2,2,1]);
    gui = gui.addView(@TrackPositionView, 'TrackPosition', 'figure', fig, 'position', [2,2,3]);
    gui = gui.addView(@MotorCurrentView, 'i_rl', 'figure', fig, 'position', [2,2,2]);
    gui = gui.addView(@MotorCurrentView, 'i_rr', 'figure', fig, 'position', [2,2,4]);
end

