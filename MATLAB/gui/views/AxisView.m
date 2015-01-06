classdef AxisView < GuiView
    %ChartView Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % attribute = char.empty; % attribute id
        % session   = []; % handle of session view is attached to
        % record    = []; % handle of record view is attached to
        % hfig      = []; % handle of figure (layout) view belongs to
        haxis       = []; % handle of axis
        atitle      = char.empty; % axis title
        axlabel     = char.empty; % x-axis label
        aylabel     = char.empty; % y-axis label
        elegend      = []; % legend entries
    end
    
    methods
        % AxisView constructor
        function obj = AxisView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'session');
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            addParameter(p, 'xlim', 'auto');
            addParameter(p, 'ylim', 'auto');
            addParameter(p, 'legend', []);
            parse(p, session, attribute, varargin{:});
            
            % initialise GuiView
            obj = obj@GuiView(...
                p.Results.session,...
                p.Results.attribute,...
                'figure', p.Results.figure...
            );
            
            % initialise AxisView
            obj.haxis = p.Results.position;
            if (isempty(obj.haxis)) % no handle/subplot pos. specified
                obj.haxis = subplot(1,1,1); % set a default
            elseif (length(obj.haxis) == 3) % subplot pos. specified
                obj.haxis = subplot(... % convert to handle
                    obj.haxis(1),...
                    obj.haxis(2),...
                    obj.haxis(3)...
                );
            end
            
            % label axes
            obj = obj.label(...
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
        
            % set axes limits
            xlim(obj.haxis, p.Results.xlim);
            ylim(obj.haxis, p.Results.ylim);
            
            % save legend entries if supplied
            obj.elegend = p.Results.legend;
        end
        
        % select axis
        function select(obj)
            % select@GuiView; % focus view figure
            axes(obj.haxis);  % focus view axis
        end
        
        % label axis
        function obj = label(obj, varargin)
            % parse input
            p = inputParser;
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, varargin{:});
            
            % apply labels
            if (~isempty(p.Results.title)) % if label supplied
                obj.atitle = p.Results.title;  % store new label
            end
            title(obj.haxis, obj.atitle); % apply stored label
            
            if (~isempty(p.Results.xlabel))
                obj.axlabel = p.Results.xlabel;
            end
            xlabel(obj.haxis, obj.axlabel);
            
            if (~isempty(p.Results.ylabel))
                obj.aylabel = p.Results.ylabel;
            end
            ylabel(obj.haxis, obj.aylabel);
        end
        
        % enable/disable legend
        function obj = clegend(obj, action)
            switch (action)
                case 'on'
                    if ~isempty(obj.elegend)
                        legend(obj.haxis, obj.elegend, 'Location', 'northwest');
                        legend(obj.haxis, 'show');
                    end        
                    
                case 'off'
                    legend(obj.haxis, 'hide');
            end
        end
    end
    
end

