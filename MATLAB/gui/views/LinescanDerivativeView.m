classdef LinescanDerivativeView < LineChartView
    %LinescanDerivativeView
    
    properties
        % attribute = char.empty; % attribute id
        % session   = []; % handle of session view is attached to
        % record    = []; % handle of record view is attached to
        % hfig      = []; % handle of figure (layout) view belongs to
        % haxis     = []; % handle of axis
        % atitle    = char.empty; % axis title
        % axlabel   = char.empty; % x-axis label
        % aylabel   = char.empty; % y-axis label
        % hplot     = []; % plot handle
        % period    = single.empty; % chart period (s) [REDUNDANT] 
    end
    
    methods
        % LinescanDerivativeView constructor
        function obj = LinescanDerivativeView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'session');
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            % addParameter(p, 'title', char.empty, @ischar);
            % addParameter(p, 'xlabel', char.empty, @ischar);
            % addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, session, attribute, varargin{:});
            
            % initialise LineChartView
            obj = obj@LineChartView(...
                p.Results.session,...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', ['Linescan Camera Image (Derivative): ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Position (px)',...
                'ylabel', 'Intensity (%/100)'...
            );
        end
    end
    
    % View update methods
    methods
        % update chart - overload update@LineChartView
        function obj = update(obj)
            % set up data
            [~,y] = obj.record.peek();
            y = transpose(y);
            dy = y(2:end) - y(1:end-1);
            
            if (~isempty(y))
                x = 1:1:127;
            else
                x = [];
                dy = [];
            end
            
            % draw chart
            obj = obj.draw(x,dy);
        end
    end
    
end

