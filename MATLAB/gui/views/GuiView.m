classdef GuiView
    %GuiView
    
    properties
        attribute = char.empty; % attribute id
        hfig      = []; % handle of figure (layout) view belongs to
    end
    
    methods
        % GuiView constructor
        function obj = GuiView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired(p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            parse(p, attribute, varargin{:});
            
            % initialise GuiView
            obj.attribute = p.Results.attribute; % attribute id
            
            obj.hfig = p.Results.figure;
            if(isempty(obj.hfig))
                fh = findall(0,'type','figure'); % find all figure handles
                if (isempty(fh))
                    fh = 0;
                end
                nf = max(fh(find(fh == fix(fh)))); % maximal integer value is number of figures
                obj.hfig = nf + 1;
            end
            obj.hfig = figure(obj.hfig); % figure handle
            
        end
        
        % select
        function select(obj)
            % select plot
            figure(obj.hfig);
        end
    end

end

