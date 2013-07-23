#ifndef __CCRANGE__
#define __CCRANGE__

   // range
   // describe a range of values, to be used to set values of arp_runner 
   // instance variables.
   // Designed to be able to scale MIDI Continuous Controller values,
   // (ranging from 0 to 127) to the correct value based on range.hi
   // and range.lo
   //
   // arp_runner will keep an array of 128 of these - one for each 
   // possible MIDI Continuous Controller type.
   //
   
   template<class parent> struct range {

      int hi,        // max value of range
          lo,        // min value of range
          toggle;    // is this simply a toggle between min and max?

      double opt;    // optimization:  store (127 / (hi - lo)) so we don't 
                     //                have to calculate it every time.
      
      parent *pr;
      void (parent::*modifier)(int);
      
      range<parent>(): hi(127), lo(0), toggle(0), opt(1), pr(NULL), modifier(NULL) {}
      
      range<parent>(int h, int l, int t, parent * p, void (parent::*m)(int))
            : hi(h), lo(l), toggle(t), pr(p), modifier(m) { 
         opt = (double)(127.0f / (hi - lo)); 
      }
      
      range<parent>( const range<parent> & rhs )
            : hi(rhs.hi), lo(rhs.lo), toggle(rhs.toggle), pr(rhs.pr), modifier(rhs.modifier) { 
         opt = (double)(127.0f / (hi - lo)); 
      }
      
      range<parent> & operator = ( const range<parent> & rhs ) {
         hi = rhs.hi;
         lo = rhs.lo;
         toggle = rhs.toggle;
         opt = (double)(127.0f / (hi - lo));
         modifier = rhs.modifier;
         pr = rhs.pr;
         return *this;
      }
      
      int scaleinput( int ccval ) {
         int retval = 0;
         if ( toggle ) {
            retval = ccval ? hi : lo;
         } else {
            if ( !opt ) opt = 1;         
            retval = (ccval / opt) + lo;
         }
         if ( pr && modifier ) {
            (pr->*modifier)(retval);
         }
         return retval;
      }
   };

#endif
