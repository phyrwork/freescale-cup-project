classdef LineChartView < AxisView
    %LineChartView
    
    properties
        hplot = []; % plot handle
    end

    methods
        % LineChartView constructor
        function obj = LineChartView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, attribute, varargin{:});
            
            % initialise AxisView
            obj = obj@AxisView(...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
        end
        
        % draw chart
        function obj = draw(obj, x, y)
            if (isempty(obj.hplot)) % initialise plot
                obj.hplot = plot(obj.haxis, x, y);
                obj = label(obj);
            else % replace plot data
                set(obj.hplot, 'XData', x, 'YData', y);
            end
        end
    end
    
end

