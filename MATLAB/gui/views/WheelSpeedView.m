classdef WheelSpeedView < ChartView
    %MotorCurrentView
    
    methods
        % constructor
        function obj = WheelSpeedView(session, actual, target, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Speed (RPM)', ...
                'ybotc', 0, ...
                'ytopu', 1000 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, actual, 'label', 'Actual', 'color', 'blue');
            obj = obj.addSeries(@ChartSeries, target, 'label', 'Command', 'color', 'green');
        end
    end
end