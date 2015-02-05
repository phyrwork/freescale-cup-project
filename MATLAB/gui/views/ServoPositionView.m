classdef ServoPositionView < ChartView
    %ServoPositionView
    
    methods
        % constructor
        function obj = ServoPositionView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Servo position (?)', ...
                'ylabel', 'Time (s)', ...
                'xbotc', -1, ...
                'xtopc', 1 ...
                );
            
            % add series
            obj = obj.addSeries(@VerticalChartSeries, attribute, 'label', 'Position', 'style', 'scatter');
        end
    end
end