classdef WheelSpeedView < ChartView
    %MotorCurrentView
    
    methods
        % constructor
        function obj = WheelSpeedView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Speed (RPM)', ...
                'ybotc', 0, ...
                'ytopu', 1000 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, attribute, 'label', 'Rotational Frequency');
        end
    end
end