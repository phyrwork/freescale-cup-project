function gui = WheelSpeedsLayout( gui, fig )
%VisionCurrentLayout Load a simple GUI window showing car vision and motor
%load current.
    
    gui = gui.addViews(...
        WheelSpeedView(gui.session, 'w_rl', 'figure', fig, 'position', [2,1,1]),...
        WheelSpeedView(gui.session, 'w_rr', 'figure', fig, 'position', [2,1,2]),...
    );
end

