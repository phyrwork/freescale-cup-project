classdef CornerProximityView < ChartView
    %MotorCurrentView
    
    methods
        % constructor
        function obj = CornerProximityView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Percieved distance to corner (m)', ...
                'ytopu', 4, ...
                'ytopl', 2, ...
                'ybotc', 0 ...��
                );
            
            % add series
            obj = obj.addSeries(@ChartSeries, attribute, 'label', 'Proximity');
        end
    end
end