classdef LineMatchView < ChartView
    %LineMatchView
    
    methods
        % constructor
        function obj = LineMatchView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'xlabel', 'Time (s)', ...
                'ylabel', 'Match probability', ...
                'ybotc', 0, ...
                'ytopc', 1 ...
                );
            
            % add series
            obj = obj.addSeries(@AslMatchSeries, attribute);
            obj = obj.addSeries(@RelMatchSeries, attribute);
            obj = obj.addSeries(@FshMatchSeries, attribute);
        end
    end
end