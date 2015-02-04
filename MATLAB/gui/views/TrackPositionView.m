classdef TrackPositionView < ChartView
    %TrackPositionView
    
    methods
        % constructor
        function obj = TrackPositionView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Perceieved track position (ahead) relative to car (px)', ...
                'ylabel', 'Time (s)', ...
                'xbotu', -64, ...
                'xtopl', 64 ...
                );
            
            % add series
            obj = obj.addSeries(@VerticalChartSeries, attribute, 'label', 'Position', 'style', 'scatter');
        end
    end
end