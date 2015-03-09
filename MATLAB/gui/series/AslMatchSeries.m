classdef AslMatchSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = AslMatchSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    'label', 'Full match', ...
                    'style', 'line', ...
                    'color', 'blue' ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [x, s] = obj.record.latest(obj.view.period);
            s = [s(:).P.asl];
            
            % draw the series
            obj = obj.draw(x, s);
        end
    end
end
