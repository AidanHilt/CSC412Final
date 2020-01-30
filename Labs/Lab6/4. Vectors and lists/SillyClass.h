/*
 *  SillyClass.h
 *  Templates
 *
 *  Created by jean-yves hervé on 2009/03/11.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 
 #include <string>

class SillyClass {

	public:
	
		SillyClass(int m, int n);
		
		~SillyClass(void);

		int getM(void) const;
		
		int getN(void) const;
		
		void setM(int m);
		
		void setN(int n);
		
		std::string toString(void) const;
		
		bool operator >= (const SillyClass& b) const;


	private:
	
		int m_; 
		int n_;


};

bool operator < (const SillyClass& a, const SillyClass& b);

