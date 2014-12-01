classdef LinescanView < LineChartView
    %LinescanView
    
    methods
        % LinescanView constructor
        function obj = LinescanView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            % addParameter(p, 'title', char.empty, @ischar);
            % addParameter(p, 'xlabel', char.empty, @ischar);
            % addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, attribute, varargin{:});
            
            % initialise LineChartView
            obj = obj@LineChartView(...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', ['Linescan Camera Image: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Position (px)',...
                'ylabel', 'Intensity (%/100)'...
            );
        end
        
        % draw chart 
        function obj = draw(obj, values)
            % set up data and plot
            x = 1:128;
            obj = draw@LineChartView(obj, x, values(1:128,1));
        end
    end
    
end

