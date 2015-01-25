classdef GuiView
    %GuiView
    
    properties
        attribute = {}; % attribute ids
        session  = []; % handle of session view is attached to
        record   = TftpRecord.empty; % handle of record view is attached to
        hfig     = []; % handle of figure (layout) view belongs to
    end
    
    methods
        % GuiView constructor
        function obj = GuiView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired(p, 'session');
            addRequired(p, 'attribute', @isattribute);
            addParameter(p, 'figure', double.empty);
            parse(p, session, attribute, varargin{:});
            
            % initialise GuiView
            obj.session = p.Results.session; % session handle
            
            if ischar(p.Results.attribute) % if only one attribute specified (i.e. specified in the regular way)
                obj.attribute{1} = p.Results.attribute;
            else
                obj.attribute = p.Results.attribute;
            end
            
            for i = 1:length(obj.attribute)
                obj.record(i) = obj.session.findAttribute(obj.attribute{i}); % retrieve record handle
                if isempty(obj.record(i))
                    warning(['Attribute ''',obj.attribute{i},''' not found in session.']);
                end
            end
            
            obj.hfig = p.Results.figure;
            if(isempty(obj.hfig))
                obj.hfig = obj.newFigure();
            else
                obj.hfig = figure(obj.hfig); % figure handle     
            end
        end
    end
    
    methods (Static)  
        % open a new figure
        function fig = newFigure()
            fh = findall(0,'type','figure'); % find all figure handles
            if (isempty(fh))
                fh = 0;
            end
            nf = max(fh(find(fh == fix(fh)))); % maximal integer value is number of figures
            fig = figure(nf + 1);
        end
    end

end

