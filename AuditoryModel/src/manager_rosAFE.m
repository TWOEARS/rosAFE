classdef manager_rosAFE < handle
%MANAGER Processor managing class for the auditory front-end (AFE) framework. A manager 
%   object controls the processing of the AFE framework. It is responsible for 
%   instantiating the required processors as well as correctly routing their respective 
%   inputs/outputs, given a request from the user. In addition, the manager methods allow 
%   the user to request a new representation or ask for the processing to be performed. 
%   Hence, the manager object represents the core of the AFE framework. 
%
%   MANAGER properties:
%       Processors - Cell array of processor objects.
%       InputList  - Handles to the input of each processors.
%       OutputList - Handles to the output of each processors.
%       Data       - Handle to the data object containing all computed signals.
%
%   MANAGER methods:
%       manager       - Constructor for the class. Requires a dataObject instance.
%       addProcessor  - Request a new auditory representation to extract.
%       processSignal - Requests the (offline) processing of an input signal.
%       processChunk  - Requests the (online) processing for a new chunk of signal..
%       hasProcessor  - Test if a given processor is already instantiated.
%       reset         - Resets internal states of all processors.
%       
%   See also dataObject, requestList, parameterHelper
%
% Disclamer: Known limitations that will be addressed in future releases
%   - When a processor becomes obsolete, its instance is not cleared from memory
%   - Few processors are not fully compatible with chunk-based processing (will return
%     erroneous representations in the vicinity of chunk boundaries):
%       * IHC methods involving Hilbert envelope extraction ('hilbert', 'joergensen', 
%         and 'bernstein')
%       * Spectro-temporal modulation extraction
%   - Pitch estimation might (though highely unlikely) be misestimated at chunk boundaries
%     in chunk-based processing scenarios
    
    
    properties (SetAccess = protected)
        % Processors - Cell array of processor objects. First column of the array contains
        % processors in charge of the left (or single) channel, second column of the
        % right channel. Different lines in the array are for different processor
        % instances.
        Processors = {};
        
        RosAFE
        
        % InputList - Cell array of handles to the input signal of each processors. A
        % signal at a given position in the array is the input to the processor stored
        % at the same position in the Processors property.
        InputList       
        
        % OutputList - Cell array of handles to the output signal of each processors. A
        % signal at a given position in the array is the output from the processor stored
        % at the same position in the Processors property.
        OutputList
        
    end
    
    methods
        function mObj = manager_rosAFE(rosAFEArg, request,p)
            %manager (RosAFE)    Constructs a manager object
            %
            %USAGE
            %     mObj = manager(request)
            %     mObj = manager(request,p)
            %
            %INPUT ARGUMENTS
            %  request : Single request as a string (e.g., 'ild'), OR cell array of
            %            requested signals, cues or features.
            %        p : Single parameter structure, if all requests share the same 
            %            parameters, OR cell array of individual parameter structures 
            %            corresponding to each request.
            %
            %OUTPUT ARGUMENTS
            %     mObj : Manager instance
            %
            %EXAMPLE USE (given an instance of dataObject, dObj)
            %- Single request, default parameters:
            %   mObj = manager(rosAFEArg,'autocorrelation')
            %- Multiple request with same parameters
            %   mObj = manager(rosAFEArg,{'ild','itd'},genParStruct('fb_nChannels',16))
            %  
            %
            
            if nargin>0     % Failproof for Matlab empty calls
            
            % Input check
            if nargin<3||isempty(p);p=[];end
            if nargin<2
                request = [];
            end
            if nargin<1
                error(['Too few arguments, the manager is built upon '...
                    'an existing data Object'])
            end
            
            setRosAFEHandle(mObj, rosAFEArg);
            updateProcessorStatus ( mObj );
            
            % Instantiate the requested processors
            if ~isempty(request)
                if iscell(request) && numel(request) == 1
                    % Then we have a one request with multiple parameters
                    if iscell(p)
                        %... with individual parameters
                        for ii = 1:size(p,2)
                            mObj.addProcessor(request,p{ii});
                        end
                    else
                        mObj.addProcessor(request,p);
                    end
                elseif iscell(request)
                    % Then we have a multiple request...
                    if iscell(p)
                        %... with individual parameters
                        if size(request,2)~=size(p,2)
                            error('Number of requests and number of provided parameters do not match')
                        else
                            for ii = 1:size(request,2)
                                mObj.addProcessor(request{ii},p{ii});
                            end
                        end
                    else
                        %... all with the same set of parameters
                        for ii = 1:size(request,2)
                            mObj.addProcessor(request{ii},p);
                        end
                    end
                elseif iscell(p)
                    % Then it is a same request but with multiple parameters
                    for ii = 1:size(p,2)
                        mObj.addProcessor(request,p{ii});
                    end
                else
                    % Then it is a single request
                     mObj.addProcessor(request,p);
                end
            end
            end
        end
        
        function hProc = hasProcessor(mObj,type,p)
            %hasProcessor (RosAFE)    Determines if a processor with a given set of parameters
            %                         (including those of its dependencies) is already instantiated
            %
            %USAGE
            %   hProc = mObj.hasProcessor(name,p)
            %   hProc = mObj.hasProcessor(name,p,channel)
            %
            %INPUT ARGUMENTS
            %    mObj : Instance of manager object
            %    name : Name of processor
            %       p : Complete structure of parameters for that processor
            %
            %OUTPUT ARGUMENT
            %   hProc : Handle to an existing processor, if any, 0 else
            
            %TODO: Will need maintenance when introducing processors with multiple lower
            %dependencies
            
            updateProcessorStatus ( mObj );

            % Initialize the output
            hProc = 0;
            
            [result, hProc] = verifyParameters( mObj, type, p );
            
        end
        
        function [out,varargout] = addProcessor(mObj,request,p)
            %addProcessor (RosAFE)   Add new processor(s) needed to compute a user request.
            %                        Optionally returns a handle to the corresponding output signal
            %
            %USAGE:
            %           mObj.addProcessor(request,p)
            %    sOut = mObj.addProcessor(...)
            %
            %INPUT ARGUMENTS
            %    mObj : Manager instance
            % request : Requested signal (string)
            %       p : Structure of non-default parameters
            %
            %OUTPUT ARGUMENTS
            %    sOut : Handle to the requested signal
            %
            %EXAMPLE USE
            %- Single request, default parameters:
            %   sOut = mObj.addProcessor('autocorrelation');
            %- Multiple request with same non-default parameters
            %   [sOut1,sOut2] = manager({'ild','itd'}, genParStruct('fb_nChannels',16));
           
%             if nargin<3 || isempty(p)
%                 % Initialize parameter structure
%                 p = struct;
%             end
            
            if nargin<3; p = []; end
                

            % Deal with multiple requests via pseudo-recursion
            if iscell(request) || iscell(p)
                
                if iscell(request) && ~iscell(p)
                    % All the requests have the same parameters, replicate
                    % them
                    p = repmat({p},size(request));
                elseif ~iscell(request) && iscell(p)
                    % One request with different parameters, replicate the request
                    request = repmat({request},size(p));
                end
                
                if size(p,2)~=size(request,2)
                    error(['Provided number of parameter structures'...
                        ' does not match the number of requests made'])
                end
                
                % Call addProcessor method for each individual request
                varargout = cell(1,size(request,2)-1);
                out = mObj.addProcessor(request{1},p{1});
                for ii = 2:size(request,2)
                    varargout{ii-1} = mObj.addProcessor(request{ii},p{ii});
                end
                return
                
            end
            
            % Find most suitable initial processor for that request
            [initProc,dep_list] = mObj.findInitProc(request,p);
            
            % Add the input processor if the initProc is empty
            if isempty(initProc)
                dep_list = [dep_list 'input'];
            end
            
            % The processing order is the reversed list of dependencies
            dep_list = fliplr(dep_list);
            
            % Initialize pointer to dependency 
            dependency = initProc;
            
            % Processors instantiation and data object property population
            for ii = 1:length(dep_list)
                switch ( cell2mat(dep_list(ii)) )
                    case ( 'input' )
                        addSingleProcessor( mObj, 'input', strcat(dep_list(ii)), ' ', p );
                        dependency = hasProcessor ( mObj, 'input', p );
                    case ( 'time' )
                        addSingleProcessor( mObj, 'preProc', strcat(dep_list(ii),'_',dependency.name), dependency.name, p );
                        dependency = hasProcessor ( mObj, 'preProc', p );                   
                    case ( 'filterbank' )
                        addSingleProcessor( mObj, 'gammatoneProc', strcat(dep_list(ii),'_',dependency.name), dependency.name, p );
                        dependency = hasProcessor ( mObj, 'gammatoneProc', p );
                    case ( 'innerhaircell' )
                        addSingleProcessor( mObj, 'ihcProc', strcat(dep_list(ii),'_',dependency.name), dependency.name, p );
                        dependency = hasProcessor ( mObj, 'ihcProc', p );
                    case ( 'ild' )
                        addSingleProcessor( mObj, 'ildProc', strcat(dep_list(ii),'_',dependency.name), dependency.name, p );
                        dependency = hasProcessor ( mObj, 'ildProc', p );
                end
            end

        end
        
        function cleanup(mObj)
            %CLEANUP (RosAFE)   Deletes all running processors except the
            % 'input'
            
            %deleteProcessor( mObj, mObj.Processors.paramsInputProc{1}.name);
            mObj.RosAFE.RemoveProcessor('-a',mObj.Processors.paramsInputProc{1}.name); % deleting the input gives an exception
        end

        function deleteProcessor( mObj, procName )
            % deleteProcessor (RosAFE) 
            %            
            remove = mObj.RosAFE.RemoveProcessor('-a', procName);
            
            updateProcessorStatus ( mObj );
            
            if ~(isempty(remove.exception))
                warning(remove.exception.ex)
            end
        end        

        function ModifyParameter( mObj, procName, paramName, newValue )
            % ModifyParameter (RosAFE) 
            %            
            modifParam = mObj.RosAFE.ModifyParameter(procName, paramName, newValue);
            
            updateProcessorStatus ( mObj );
            
            if ~(isempty(modifParam.exception))
                warning(modifParam.exception.ex)
            end
        end   
        
        function reset(mObj)
            %reset  Resets the internal states of all instantiated processors
            %
            %USAGE:
            %  mObj.reset
            %
            %INPUT ARGUMENTS
            %  mObj : Manager instance
            
            % Is the manager working on a binaural signal?

            
        end
        
    end
    
    methods (Access = protected)
       
        function [hProc,list] = findInitProc(mObj,request,p)
            %findInitProc (RosAFE)  Find an initial compatible processor for a new
            %                       request
            %
            %USAGE:
            %         hProc = mObj.findInitProc(request,p)
            %  [hProc,list] = mObj.findInitProc(request,p)
            %
            %INPUT PARAMETERS
            %    mObj : Manager instance
            % request : Requested signal name
            %       p : Parameter structure associated to the request
            %
            %OUTPUT PARAMETERS
            %   hProc : Handle to the highest processor in the processing 
            %           chain that is compatible with the provided
            %           parameters. In case two instances exist for the
            %           processor for a stereo signal, hProc is a cell
            %           array of the form {'leftEarProc','rightEarProc'}
            %    list : List of signal names that need to be computed,
            %           starting from the output of hProc, to obtain the
            %           request

            dep_list = [request ...
                Processor.getDependencyList( ...
                Processor.findProcessorFromRequest(request,p), p)];
            
            
            % Initialization of while loop
            ii = 1;
%             dep = signal2procName(dep_list{ii},p);
            dep = Processor.findProcessorFromRequest(dep_list{ii},p);
            hProc = mObj.hasProcessor(dep,p);
                stop = 0;
                try
                    if (hProc == 0)
                        stop = 0;
                    end
                catch ME
                    if (strcmp(ME.identifier,'MATLAB:UndefinedFunction'))
                        stop = 1;
                    end
                end
            list = {};
            
            % Looping until we find a suitable processor in the list of
            % dependency
            while stop == 0 && ii<size(dep_list,2)
                
                % Then we will need to re-compute that signal
                list = [list dep_list{ii}]; %#ok
                
                % Move on to next level of dependency
                ii = ii + 1;
                dep = Processor.findProcessorFromRequest(dep_list{ii},p);
                hProc = mObj.hasProcessor(dep,p);
                try
                    if (hProc == 0)
                        stop = 0;
                    end
                catch ME
                    if (strcmp(ME.identifier,'MATLAB:UndefinedFunction'))
                        stop = 1;
                    end
                end
            end
            
            if stop == 0
                % Then all the signals need recomputation, including time
                list = [list dep_list{end}];
                
                % Return a empty handle
                hProc = [];
            end
            
        end
        
        function setRosAFEHandle ( mObj, rosAFEArg )
            % setRosAFEHandle (RosAFE)      Sets a hangle to the rosAFE
            %                               genom component
            mObj.RosAFE = rosAFEArg;
        end
        
        function addSingleProcessor( mObj, procType, procName, dependencies, par )
            %addSingleProcessor (RosAFE)     Instantiates a new processor and integrates it to the
            %                                manager instance

            switch ( procType )
                case ('input')
                    thisProc = mObj.RosAFE.InputProc('-a', strcat(cell2mat(procName),'_',int2str((length(mObj.Processors.paramsInputProc)))), 12000, 10 );
                
                case ('preProc')
                    defaultParameters = Parameters.getProcessorDefault( procType );
                    thisProc = mObj.RosAFE.PreProc('-a', strcat(cell2mat(procName),'_',int2str((length(mObj.Processors.paramsPreProc)))), dependencies, 44100, ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bRemoveDC'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_cutoffHzDC'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bPreEmphasis'), ...  
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_coefPreEmphasis'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bNormalizeRMS'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bBinauralRMS'), ...                                                              
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_intTimeSecRMS'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bLevelScaling'), ...                                                                        
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_refSPLdB'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bMiddleEarFiltering'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_middleEarModel'), ...                                                                        
                                                                                   defaultOrAsked(par, defaultParameters, 'pp_bUnityComp')  );  
                case ('gammatoneProc')
                    defaultParameters = Parameters.getProcessorDefault( procType );
                    thisProc = mObj.RosAFE.GammatoneProc('-a', strcat(cell2mat(procName),'_',int2str((length(mObj.Processors.paramsGammatoneProc)))), ...
                                                                                            dependencies, 44100,    defaultOrAsked(par, defaultParameters, 'fb_type'), ...
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_lowFreqHz'), ...                                                              
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_highFreqHz'), ...
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_nERBs'), ...                                                                        
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_nChannels'), ...
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_cfHz'), ...
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_nGamma'), ...                                                                        
                                                                                            defaultOrAsked(par, defaultParameters, 'fb_bwERBs')  );                    
                case ('ihcProc')
                    defaultParameters = Parameters.getProcessorDefault( procType );
                    thisProc = mObj.RosAFE.IhcProc('-a', strcat(cell2mat(procName),'_',int2str((length(mObj.Processors.paramsIhcProc)))), dependencies, 44100, defaultOrAsked(par, defaultParameters, 'ihc_method') ); 
                case ('ildProc')
                    defaultParameters = Parameters.getProcessorDefault( procType );
                    thisProc = mObj.RosAFE.IldProc('-a', strcat(cell2mat(procName),'_',int2str((length(mObj.Processors.paramsIldProc)))), ...
                                                                                   dependencies, 44100, defaultOrAsked(par, defaultParameters, 'ild_wname'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'ild_wSizeSec'), ...
                                                                                   defaultOrAsked(par, defaultParameters, 'ild_hSizeSec') );                    
                otherwise
                    error('Processor type doesnt exist');
            end
            

            updateProcessorStatus ( mObj );
            
            if ~(isempty(thisProc.exception))
                warning(thisProc.exception.ex)
            end
            
            % Instantiate and integrate new output signal
            %output = newProcessor.instantiateOutput(mObj.Data);
            %newProcessor.addOutput(output);
            
        end
        
        function updateProcessorStatus ( mObj )
            % updateProcessorStatus (RosAFE)    Reads the published ROS
            %                                   topic about the processors status
            
            pause(0.2);
            rp = mObj.RosAFE.runningProcessors();
            mObj.Processors = rp.runningProcessors;
        end
    end
    
end