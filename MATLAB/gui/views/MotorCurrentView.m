classdef MotorCurrentView < ChartView
    %MotorCurrentView
    
    methods
        % constructor
        function obj = MotorCurrentView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Current (A)', ...
                'ybotc', 0, ...
                'ytopl', 0.5 ...
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, attribute, 'label', 'Current');
        end
    end
end