classdef LinescanView < LineChartView
    %LinescanView
    
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
        % LinescanView constructor
        function obj = LinescanView(session, attribute, varargin)
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
                'title', ['Linescan Camera Image: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Position (px)',...
                'ylabel', 'Intensity (Raw 16-bit)',...
                'legend', {'Intensity','Detected Edges (Modelled)'}...
            );
        end
    end
    
    % View update methods
    methods
        % draw chart - overload draw@LineChartView
        function obj = draw(obj, lx, ly, sx, sy)
            % draw
            if (isempty(obj.hplot)) % initialise plot
                obj.hplot = plot(obj.haxis, lx, ly); % plot linescan image
                hold on
                obj.hplot = [obj.hplot, scatter(obj.haxis, sx, sy, 'r', '*')]; % plot model detected edges
                hold off
                obj = label(obj);
                if (~isempty(obj.hplot)) % only call legend if plot exists to surpress warnings
                    obj.clegend('on');
                end
            else % replace plot data
                set(obj.hplot(1), 'XData', lx, 'YData', ly); % replace linescan image
                set(obj.hplot(2), 'XData', sx, 'YData', sy); % replace model detected edges
            end
        end
        
        % update chart - overload update@LineChartView
        function obj = update(obj)
            
            % set up data
            [~,ly] = obj.record.peek();
            ly = transpose(ly);
            
            if (~isempty(ly))
                % prep linescan x-axis
                lx = 1:1:128;
                
                % prep model detected edges
                edges = findEdges(ly);
                sx = [];
                for i = 1:length(edges)
                    sx = [sx, edges(i).pos];
                end
                sy = ly(sx);
            else
                lx = [];
                ly = [];
                sx = [];
                sy = [];
            end
            
            % draw chart
            obj = obj.draw(lx,ly,sx,sy);
            ylim(obj.haxis, [0, 4095]);
            xlim(obj.haxis, [1, length(ly)]); % adjust y-axis limits
        end
    end
    
end

