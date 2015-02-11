classdef WheelSpeedView < ChartView
    %MotorCurrentView
    
    methods
        % constructor
        function obj = WheelSpeedView(session, actual, varargin)
            
            % parse arguments
            p = inputParser;
            p.KeepUnmatched = true;
            addParameter(p, 'target', char.empty, @ischar);
            parse(p, varargin{:});
            target = p.Results.target;
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Speed (rev/s)', ...
                'ybotc', 0, ...
                'ytopu', 30 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, actual, 'label', 'Actual', 'color', 'blue');
            if ~isempty(target)
                obj = obj.addSeries(@ChartSeries, target, 'label', 'Command', 'color', 'green');
            end
        end
    end
end