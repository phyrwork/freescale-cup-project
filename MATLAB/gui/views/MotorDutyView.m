classdef MotorDutyView < ChartView
    %MotorDutyView
    
    methods
        % constructor
        function obj = MotorDutyView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Duty (*100%)', ...
                'ytopc', 1, ...
                'ybotc', -1 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, attribute);
        end
    end
end