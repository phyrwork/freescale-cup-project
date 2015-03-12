function gui = CornerProximityLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LinescanView, 'linescan1', 'figure', fig, 'position', [2,1,1]);
    gui = gui.addView(@CornerProximityView, 'CornerProximity', 'figure', fig, 'position', [2,1,2]);
end

