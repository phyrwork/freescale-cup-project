classdef PidView < LineChartView
    %PidView
    
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
        % period    = single.empty; % chart period (s)
    end
    
    methods
        % PidView constructor
        function obj = PidView(session, attribute, varargin)
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
                'title', ['PID controller internals: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Time (s)',...
                'legend', {'Output','Error (abs)'}...
            );
        end
    end
    
    % View update methods
    methods
        % update chart - overload update@LineChartView
        function obj = update(obj)
            % fetch and prepare data
            [x,samples] = obj.record.latest(obj.period);
            y = zeros(2,length(samples), 'single');
            for i = 1:length(samples)
                y(1,i) = samples(i).value; % series 1
                y(2,i) = samples(i).error; % series 2
            end
            
            % draw chart
            obj = obj.draw(x, y);
        end
    end
    
end
