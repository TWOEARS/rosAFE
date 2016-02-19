#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <stdint.h>
#include <vector>
#include <string> 

#include <iostream> 
#include <memory>

#include <apf/parameter_map.h>

#include "circularContainer.hpp"

namespace openAFE {
	
	enum channel {
		_mono,
		_left,
		_right
	};
	
	/* This all templated signals are derivated from this non-templated base class */ 
	class SignalBase {
		
	public :
		
		SignalBase() {}
		virtual ~SignalBase() {}
		
		// virtual void appendChunk( ) = 0;
		
		virtual void printSignal() {}
	};
	
	/* This is a shared pointer to the signal objects */
	typedef std::shared_ptr<SignalBase > signalBaseSharedPtr;
	  
	template<typename T>
	class Signal : public SignalBase {

	protected:
	
	    typedef std::unique_ptr<CircularContainer<T> > containerPtr;
	    typedef std::vector<containerPtr > bufferPtrVector;
	    typedef typename std::vector<containerPtr >::iterator bufferIter;
	    
	    typedef std::shared_ptr<nTwoCTypeBlockAccessor<T> > nTwoCTypeBlockAccessorPtr;

		stringP Label;           			// Used to label the signal (e.g., in plots)
		stringP Name;            			// Used as an instance name
		stringP Dimensions;      			// String describing the dimensions of the signal
		unsigned int  FsHz;       			// Sampling frequency
		channel Channel;         			// Flag keeping track of the channel

		uint64_t bufferSizeSamples;
		uint64Vector bufferElemSize;
		
		bufferPtrVector bufferVector;		// The buffer(s) of the signal
		nTwoCTypeBlockAccessorPtr allLastChunkInfo, allLastDataInfo, allOldDataInfo, allWholeBufferInfo;
		
		void populateProperties(apf::parameter_map& params) {
			
			if (params.has_key("Label")) {
				stringP tmpLabel (new std::string(params["Label"]) );
				Label = std::move( tmpLabel );
			}
			if (params.has_key("Name")) {
				stringP tmpName (new std::string(params["Name"]) );
				Name = std::move( tmpName );
			}
			if (params.has_key("Dimensions")) {
				stringP tmpDimensions (new std::string(params["Dimensions"]) );
				Dimensions = std::move( tmpDimensions );
			}
		}

		void clearData() {
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it)
				(*it)->clear();
		}
				
		void calcLastChunk() {
			uint64_t i = 0;
			twoCTypeBlock<T> *tmp;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				tmp = (*it)->getLastChunk();
				allLastChunkInfo->setData (i, tmp);
				i++;
			}
		}
		
		void calcLastData( uint64_t& samplesArg ) {
			uint64_t i = 0;
			twoCTypeBlock<T> *tmp;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				tmp = (*it)->getLastData( samplesArg );
				allLastDataInfo->setData (i, tmp);
				i++;
			}
		}
		
		void calcOldData( uint64_t& samplesArg ) {
			uint64_t i = 0;
			twoCTypeBlock<T>* tmp;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				tmp = (*it)->getOldData( samplesArg );
				allOldDataInfo->setData (i, tmp);
				i++;
			}
		}

		void calcWholeBuffer() {
			uint64_t i = 0;
			twoCTypeBlock<T> *tmp;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				tmp = (*it)->getWholeBuffer();
				allWholeBufferInfo->setData (i, tmp);
				i++;
			}
		}

	public:

		/*	
		 *  INPUT ARGUMENTS:
		 *  fs : Sampling frequency (Hz)
		 *  bufferSize_s : Buffer duration in s
		 *  bufferElemSize : Additional dimensions of the buffer
		 *                   [dim2,dim3,...]
		*/
				      
		Signal(const uint64_t fs, const uint64_t bufferSize_s, const uint64Vector& bufferElemSize = uint64Vector() ) : SignalBase() {
		
		/*
		 *  If you call the constructor with just two arguments, it is automatically a vector.
		 * 	If you want 2D, 3D, 4D .. matrixes, initialise the bufferElemSize when you
		 *  call the constructor
		 */
			this->FsHz = fs;
			
			/* Calculation of the buffer size in samples */
			this->bufferSizeSamples = ceil( bufferSize_s * this->FsHz );
			this->bufferElemSize = bufferElemSize;
			
			this->init();
		}    
		
		virtual ~Signal() {
			// Buf, Label, Name and Dimensions are smart pointrs
			this->bufferVector.clear();
			std::cout << "Destructor of a signal" << std::endl;
		}
		
		void init() {
			
			/* bufferNumber = number of buffer to create */
			unsigned int bufferNumber = 1;
			if ( bufferElemSize.size() > 0 ) {
				if ( bufferElemSize.size() > 1 ) {
				for (uint64Vector::const_iterator it = bufferElemSize.begin() ; it != bufferElemSize.end(); ++it)
				 bufferNumber *=  *it;
				} else bufferNumber = *( bufferElemSize.begin() );
			}
			
			/* Buffer's pointers are stored linearly.
			 * As itearators are used, this is not mendatory. */
			this->bufferVector.reserve(bufferNumber);
			
			/* Instantiation of the 1D buffers. Buffers are smart (unique) pointers */
			for (unsigned int i = 0 ; i < bufferNumber ; ++i) {
				containerPtr newBuff( new CircularContainer<T>(bufferSizeSamples) );
				bufferVector.push_back ( std::move ( newBuff ) );
			}
			
			/* 
			 * C type pointer version for allLastChunkInfo,
			 * allLastDataInfo and for allOldDataInfo
			 * */
			
			/* This struct is to acces to the last chunk of each buffer */
			nTwoCTypeBlockAccessorPtr tmp ( new nTwoCTypeBlockAccessor<T>( bufferNumber ) );
			this->allLastChunkInfo = std::move ( tmp ) ;

			/* This struct is to acces to the last data of asked sample numbers of each buffer */			
			nTwoCTypeBlockAccessorPtr tmp2 ( new nTwoCTypeBlockAccessor<T>( bufferNumber ) );
			this->allLastDataInfo = std::move ( tmp2 ) ;

			/* This struct is to acces to the oldest data of asked sample numbers of each buffer */						
			nTwoCTypeBlockAccessorPtr tmp3 ( new nTwoCTypeBlockAccessor<T>( bufferNumber ) );
			this->allOldDataInfo = std::move ( tmp3 ) ;

			nTwoCTypeBlockAccessorPtr tmp4 ( new nTwoCTypeBlockAccessor<T>( bufferNumber ) );
			this->allWholeBufferInfo = std::move ( tmp4 ) ;			
			/* here tmp, tmp2 and tmp3 are deleted */
		}

		uint64_t getBufferNumber() {
			return this->bufferVector.size();
		}
		
		// virtual void plot() = 0;
		
		void reset () {
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it)
				(*it)->reset();
		}
		
		void setLastChunkSize( uint64_t& numSamples ) {
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it)
				(*it)->setLastChunkSize( numSamples );
		}
		
		/* We have two type of appendChunk functions.
		 * In the first case, the user of this signal have continious
		 * audio vector. This audio vector can be n dimentinal.
		 * So a struct called nDimAudioChunkAccessor should be pointed
		 * to the fisrt value of all vectors and be informed on the size
		 * of the vector. Naturally, the size of the incoming chunk is
		 * known. We circular buffer updates automatically the size of
		 * the last chunk.
		 */
		void appendChunk( nDimAudioChunkAccessor<T>& inChunk ) {
			assert( inChunk.getDimOfSignal() == bufferVector.size() );
			uint64_t i = 0;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				(*it)->push_chunk( inChunk.getT(i), inChunk.getD(i));
				i++;
			}
		}

		/* In the second case, the data is conserved in two continious
		 * c type arrays. But the two arrays migth not be continuous.
		 * This is due to the outcome of the circular buffer
		 * (look to the circularContainer.hpp to more information).
		 * In this case the two parts of the signal should be appended
		 * one after the other. Naturally this funtions bypasses the
		 * automatic setLastChunkSize funtion of the circularContainer
		 * class. The true size of the last chunk is calculated in
		 * this funtion and it is setted manually.
		 */
		void appendChunk( nTwoCTypeBlockAccessorPtr inChunk ) {
			assert( inChunk->getDimOfSignal() == bufferVector.size() );
			uint64_t i = 0;
			for(bufferIter it = bufferVector.begin(); it != bufferVector.end(); ++it) {
				/* The first array */
				(*it)->push_chunk( inChunk->getT(i), inChunk->getD(i), false);
				/* If there is any data, the second array */
				if ( inChunk->getD(i, true) > 0 )
					(*it)->push_chunk( inChunk->getT(i, true), inChunk->getD(i, true), false);
				/* The total size of the last chunk */
				(*it)->setLastChunkSize( inChunk->getD(i) + inChunk->getD(i, true) );
				
				i++;
			}
		}
		
		/* Append one dimentional cotinious data */
		virtual void appendChunk( T* inChunk, uint64_t dim, uint64_t bufferNumber = 0) {
			assert( bufferNumber <= bufferVector.size() );
			bufferIter it = bufferVector.begin();
				
			/* The first array */
			( *(it + bufferNumber) )->push_chunk( inChunk, dim );

		}

		nTwoCTypeBlockAccessorPtr getLastChunk() {
			this->calcLastChunk();
			return this->allLastChunkInfo;
		}
		
		nTwoCTypeBlockAccessorPtr getLastData( uint64_t samplesArg ) {
			this->calcLastData( samplesArg );
			return this->allLastDataInfo;
		}
		
		nTwoCTypeBlockAccessorPtr getOldData( uint64_t samplesArg = 0 ) {
			this->calcOldData( samplesArg );
			return this->allOldDataInfo;
		}

		nTwoCTypeBlockAccessorPtr getWholeBuffer() {
			this->calcWholeBuffer();
			return this->allWholeBufferInfo;
		}
				
		stringP getName(){
			return Name;
		}
		
		channel* getChannel() {  // try const
			return &Channel;
		}
				
		void printSignal() {
			std::cout << "Label : " << *(this->Label) << std::endl;
			std::cout << "Name : " << *(this->Name) << std::endl;	
			std::cout << "Dimensions : " << *(this->Dimensions) << std::endl;
			std::cout << "FsHz : " << (this->FsHz) << std::endl;				
			std::cout << "Channel : " << (this->Channel) << std::endl;				
					
		}

		/* When setting a Signal, if the inChunk is pointed to the data of 
		 * this signal, you wil lface with an error. This is not possible !
		 */ 		
		virtual void setData(nDimAudioChunkAccessor<T>& inChunk) {	
			this->clearData();
			this->bufferVector.clear();
			this->init();
			appendChunk( inChunk );	
		}

		virtual void setData(nTwoCTypeBlockAccessorPtr inChunk) {	
			this->clearData();
			this->bufferVector.clear();
			this->init();
			appendChunk( inChunk );	
		}

	};
};

#endif /* SIGNAL_HPP */

