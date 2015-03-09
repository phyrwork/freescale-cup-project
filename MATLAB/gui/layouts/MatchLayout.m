function gui = MatchLayout( gui, fig )
%MatchLayout
    
    % open a new figure
    fig = gui.newFigure;
    
    % initialise views
    % TODO - Add titles
    gui = gui.addView(@LineMatchView, 'TargetLine', 'figure', fig, 'position', [1,3,2]);
    gui = gui.addView(@LineMatchView, 'BestMatch', 'figure', fig, 'position', [1,3,3]);
    gui = gui.addView(@LinescanView, 'linescan0', 'figure', fig, 'position', [1,3,1]);
end

