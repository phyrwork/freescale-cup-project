classdef LinescanView < ChartView
    %LinescanView
    
    methods
        % constructor
        function obj = LinescanView(session, attribute, varargin)
            
            % initialise view
            obj = obj@ChartView(...
                session,varargin{:}, ...
                'ytopc', 4096, ...
                'ybotc', 0, ...
                'xtopc', 128, ...
                'xbotc', 0 ...
                );
            
            % add series
            obj = obj.addSeries(@LinescanImageSeries, attribute);
            obj = obj.addSeries(@LinescanEdgesSeries, attribute);
        end
    end
end