function [result, hproc] = verifyParameters( mObj, type, p, name)

            result = 0;
            hproc = 0;
            
            if nargin < 4
                name = [];
            end

            switch ( type )
                
                case ('input')

                    % Loop over the preProc processors
                    for ii = 1:length(mObj.Processors.paramsInputProc)

                        % Get a handle to that processor, for readability in the
                        % following
                        proc = mObj.Processors.paramsInputProc{ii};
                            
                        if ( (length(name)==0) || ( strcmp(name, proc.name) ) )
                            hproc = proc;
                            result = 1;
                        end
                    end                 
                
                
                case ('preProc')
                   
                   defaultParams = Parameters.getProcessorDefault(type);
                   % Loop over the preProc processors
                    for ii = 1:length(mObj.Processors.paramsPreProc)

                        % Get a handle to that processor, for readability in the
                        % following
                        proc = mObj.Processors.paramsPreProc{ii};
                            
                        if ( (length(name)==0) || ( strcmp(name, proc.name) ) )
                             if ( (proc.pp_bRemoveDC == defaultOrAsked(p , defaultParams, 'pp_bRemoveDC')) && ...  
                                  (proc.pp_cutoffHzDC == defaultOrAsked(p , defaultParams, 'pp_cutoffHzDC')) && ...
                                  (proc.pp_bPreEmphasis == defaultOrAsked(p , defaultParams, 'pp_bPreEmphasis')) && ...  
                                  (proc.pp_coefPreEmphasis == defaultOrAsked(p , defaultParams, 'pp_coefPreEmphasis')) && ...  
                                  (proc.pp_bNormalizeRMS == defaultOrAsked(p , defaultParams, 'pp_bNormalizeRMS')) && ...
                                  (proc.pp_bBinauralRMS == defaultOrAsked(p , defaultParams, 'pp_bBinauralRMS')) && ...
                                  (proc.pp_intTimeSecRMS == defaultOrAsked(p , defaultParams, 'pp_intTimeSecRMS')) && ...  
                                  (proc.pp_bLevelScaling == defaultOrAsked(p , defaultParams, 'pp_bLevelScaling')) && ...  
                                  (proc.pp_refSPLdB == defaultOrAsked(p , defaultParams, 'pp_refSPLdB')) && ...  
                                  (proc.pp_bMiddleEarFiltering == defaultOrAsked(p , defaultParams, 'pp_bMiddleEarFiltering')) && ...  
                                   strcmp(proc.pp_middleEarModel, defaultOrAsked(p , defaultParams, 'pp_middleEarModel')) && ...
                                  (proc.pp_bUnityComp == defaultOrAsked(p , defaultParams, 'pp_bUnityComp')) )
                              
                                   result = verifyParameters( mObj, 'input', p, proc.upperDepName);   
                                   
                                   if ( result == 1 )
                                       hproc = proc;
                                   end

                            end
                        end
                    end  
                   
                case ('gammatoneProc')
                    
                   defaultParams = Parameters.getProcessorDefault(type);
                   % Loop over the IHC processors
                    for ii = 1:length(mObj.Processors.paramsGammatoneProc)

                        % Get a handle to that processor, for readability in the
                        % following
                        proc = mObj.Processors.paramsGammatoneProc{ii};
                            
                        if ( (length(name)==0) || ( strcmp(name, proc.name) ) )
                             if ( strcmp(proc.fb_type, defaultOrAsked(p , defaultParams, 'fb_type')) && ...
                                  (proc.fb_lowFreqHz == defaultOrAsked(p , defaultParams, 'fb_lowFreqHz')) && ...  
                                  (proc.fb_highFreqHz == defaultOrAsked(p , defaultParams, 'fb_highFreqHz')) && ...
                                  (proc.fb_nERBs == defaultOrAsked(p , defaultParams, 'fb_nERBs')) && ...  
                                  (proc.fb_nGamma == defaultOrAsked(p , defaultParams, 'fb_nGamma')) && ...  
                                  (proc.fb_bwERBs == defaultOrAsked(p , defaultParams, 'fb_bwERBs')) )
                             
                                 %(proc.fb_nChannels == defaultOrAsked(p , defaultParams, 'fb_nChannels')) && ...                           
                                 %(proc.fb_cfHz == defaultOrAsked(p , defaultParams, 'fb_cfHz')) && ...
                                 
                                % Then it is a suitable candidate, we should
                                % investigate its dependencies
                                result = verifyParameters( mObj, 'preProc', p, proc.upperDepName);
                                if ( result == 1 )
                                    hproc = proc;
                                end
                            end
                        end
                    end                    
                case ('ihcProc')

                    defaultParams = Parameters.getProcessorDefault(type);
                    % Loop over the IHC processors
                    for ii = 1:length(mObj.Processors.paramsIhcProc)

                        % Get a handle to that processor, for readability in the
                        % following
                        proc = mObj.Processors.paramsIhcProc{ii};
                            
                        if ( (length(name)==0) || ( strcmp(name, proc.name) ) )
                            if ( strcmp(proc.ihc_method, defaultOrAsked(p , defaultParams, 'ihc_method')) )

                                % Then it is a suitable candidate, we should
                                % investigate its dependencies
                                result = verifyParameters( mObj, 'gammatoneProc', p, proc.upperDepName);
                                if ( result == 1 )
                                    hproc = proc;
                                end
                            end
                        end
                    end
                    
                case ('ildProc')

                    defaultParams = Parameters.getProcessorDefault(type);
                    % Loop over the ILD processors
                    for ii = 1:length(mObj.Processors.paramsIldProc)

                        % Get a handle to that processor, for readability in the
                        % following
                        proc = mObj.Processors.paramsIldProc{ii};
                            
                        if ( (length(name)==0) || ( strcmp(name, proc.name) ) )
                            if ( strcmp(proc.ild_wname, defaultOrAsked(p , defaultParams, 'ild_wname')) && ...
                                 (proc.ild_wSizeSec == defaultOrAsked(p , defaultParams, 'ild_wSizeSec')) && ...  
                                 (proc.ild_hSizeSec == defaultOrAsked(p , defaultParams, 'ild_hSizeSec')) )

                                % Then it is a suitable candidate, we should
                                % investigate its dependencies
                                result = verifyParameters( mObj, 'ihcProc', p, proc.upperDepName);
                                if ( result == 1 )
                                    hproc = proc;
                                end
                                    
                            end
                        end
                    end

                    
                otherwise
                    result = 0;
            end




end