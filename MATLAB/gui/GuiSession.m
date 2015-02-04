classdef GuiSession
    %GuiSession
    
    properties
        session = TftpSession.empty;
        views = {};
    end
    
    methods
        % constructor
        function obj = GuiSession(session, varargin)
            
            % set parameters
            obj.session = session;
            
            % add all layout arguments
            obj = obj.addLayout(varargin{:});
        end
        
        % refresh gui
        function obj = refresh(obj)
            
            % attempt to refresh all views
            i = 1;
            while i <= length(obj.views)
                try 
                    obj.views{i} = obj.views{i}.refresh;
                    i = i + 1; % successful, advance
                catch
                    obj.views(i) = []; % something went wrong, delete the view
                end
            end
        end
        
        % addView
        function obj = addView(obj, view, attribute, varargin)
            
            % call view with arguments
            obj.views{end + 1} = view(obj.session, attribute, varargin{:});
        end
        
        % addLayout
        function obj = addLayout(obj, varargin)
        
            % add all layout arguments to GUI
            for i = 1:length(varargin)
            
                % call layout function/constructor
                f = varargin{i};
                obj = f(obj);
            end
        end
    end
    
    methods (Static)
        % open a new figure
        function fig = newFigure()

            % find all figure handles
            fh = findall(0,'type','figure');
            if isempty(fh)
                nf = 0;
            else
                nf = max([fh.Number]);
            end

            fig = figure(nf + 1); % new figure is one above this
        end
    end
end

