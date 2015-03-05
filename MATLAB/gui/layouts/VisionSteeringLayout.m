function gui = VisionSteeringLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LinescanView, 'linescan0', 'figure', fig, 'position', [2,2,1]);
    gui = gui.addView(@TrackPositionView, 'TrackPosition', 'figure', fig, 'position', [2,2,2]);
    gui = gui.addView(@ServoPositionView, 'ServoPosition', 'figure', fig, 'position', [2,2,4]);
end

