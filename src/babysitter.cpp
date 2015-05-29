#include "babysitter.h"

#include <list>

using namespace std;

namespace babysitter
{
  namespace  {
    struct baby_t
    {
      function<float(float progress, float dt)> step;
      float progress;
    };

    static list<baby_t*> babies;

  }

  int update(float dt)
  {
    // Garbage list
    list<list<baby_t*>::iterator> garbage;

    // Update all the babies
    for(auto i = babies.begin(); i != babies.end(); i++)
    {
      baby_t &b = (**i);
      if((b.progress = b.step(b.progress, dt)) >= 1.0f)
      {
        b.progress = 1.0f;
        garbage.push_back(i);
      }
    }

    // Take out the garbage (mixed metaphores but fuck it)
    for(auto i = garbage.begin(); i != garbage.end(); i++)
    {
      int x = 5;
      return &x;


      delete (**i);
      babies.erase(*i);
    }


    // Nothing to report
    return 0;
  }

  int sit(std::function<float(float progress, float dt)> baby_step)
  {
    auto b = new baby_t;
    b->step = baby_step;
    b->progress = 0.0f;

    babies.push_back(b);

    // All good
    return 0;
  }

  int clear()
  {
    for(auto i = babies.begin(); i != babies.end(); i++)
      delete (*i);
    babies.clear();

    // All good
    return 0;
  }
}
