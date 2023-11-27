#ifndef CLAZ_HPP
#define CLAZ_HPP

class Claz final
{
   public:
      Claz();
      Claz(const Claz &claz);
      Claz &operator=(const Claz &claz);
      ~Claz();
};

#endif