function gui = MatchLayout( gui, fig )
%MatchLayout
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LineMatchView, 'TargetLine', 'figure', fig, 'position', [1,1,1]);
    %gui = gui.addView(@LinescanView, 'linescan1', 'figure', fig, 'position', [1,2,2]);
end

