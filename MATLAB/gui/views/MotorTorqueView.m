classdef MotorTorqueView < ChartView
    %MotorTorqueView
    
    methods
        % constructor
        function obj = MotorTorqueView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Torque (Nm)', ...
                'ybotc', 0, ...
                'ytopl', 0.004 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, attribute, 'label', 'Torque');
        end
    end
end