classdef GuiView
    %GuiView
    
    properties
        attribute = char.empty; % attribute id
        session  = []; % handle of session view is attached to
        record   = []; % handle of record view is attached to
        hfig     = []; % handle of figure (layout) view belongs to
    end
    
    methods
        % GuiView constructor
        function obj = GuiView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired(p, 'session');
            addRequired(p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            parse(p, session, attribute, varargin{:});
            
            % initialise GuiView
            obj.attribute = p.Results.attribute; % attribute id
            
            obj.session = p.Results.session; % session handle
            obj.record = obj.session.findAttribute(obj.attribute); % retrieve record handle
            if (isempty(obj.record))
                warning(['Attribute ''',obj.attribute,''' not found in session.']);
            end
            
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

