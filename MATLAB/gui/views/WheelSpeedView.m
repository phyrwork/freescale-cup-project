classdef WheelSpeedView < LineChartView
    %WheelSpeedView
    
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
        % WheelSpeedView constructor
        function obj = WheelSpeedView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'session');
            addRequired (p, 'attribute', @isattribute);
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
                'title', ['Wheel speed: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Time (s)',...
                'ylabel', 'Speed (RPM)',...
                'period', 5 ...
            );
        end
        
        % update chart - overload @LineChartView
        function obj = update(obj)
            % if no data nothing to do, return
            if (obj.record.rsize < 1)
                return;
            end
            
            % fetch data
            [x,y] = obj.record.latest(obj.period);
            
            % draw chart
            obj = obj.draw(x, y);
            if (max(y) > 0)
                ylim(obj.haxis, [0, max(y)]); % adjust y-axis limits
            end
            xlim(obj.haxis, [x(1), x(end)]); % adjust y-axis limits
        end
    end
    
end

