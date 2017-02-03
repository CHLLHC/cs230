#include "boolean.h"

// Determine if the ray intersects with the boolean of A and B.
bool Boolean::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
	// TODO
	std::vector<Hit> A_hits;
	std::vector<Hit> B_hits;
	A->Intersection(ray, A_hits);
	B->Intersection(ray, B_hits);

	std::vector<Hit>::const_iterator Acit = A_hits.begin();
	std::vector<Hit>::const_iterator Bcit = B_hits.begin();
	double t = 0;
	bool Ago;
	std::vector<Hit> localHits;

	enum STATUS
	{
		IN, OUT, ENTRY, EXIT
	};
	STATUS As = OUT, Bs = OUT;

	while (Acit != A_hits.end() || Bcit != B_hits.end())
	{
		Ago = false;
		if (Acit != A_hits.end())
		{
			t = Acit->t;
			Ago = true;
		}
		if (Bcit != B_hits.end())
		{
			if ((Bcit->t < t) || (!Ago))
			{
				Ago = false;
				t = Bcit->t;
			}
		}

		if (As == EXIT)
			As = OUT;
		if (As == ENTRY)
			As = IN;
		if (Bs == EXIT)
			Bs = OUT;
		if (Bs == ENTRY)
			Bs = IN;

		if (Ago)
		{
			if (Acit->ray_exiting)
			{
				As = EXIT;
			}
			else
			{
				As = ENTRY;
			}
		}
		else
		{
			if (Bcit->ray_exiting)
			{
				Bs = EXIT;
			}
			else
			{
				Bs = ENTRY;
			}
		}

		STATUS thisHit = OUT;
		Hit theHit;
		theHit.t = t;
		theHit.ray_exiting = false;

		switch (type)
		{
		case type_intersection:
			if ((As == IN) && (Bs == ENTRY))
			{
				thisHit = ENTRY;
				theHit.object = Bcit->object;
			}
			if ((As == IN) && (Bs == EXIT))
			{
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == IN))
			{
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == IN))
			{
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			break;
		case type_union:
			if ((As == OUT) && (Bs == ENTRY))
			{
				thisHit = ENTRY;
				theHit.object = Bcit->object;
			}
			if ((As == OUT) && (Bs == EXIT))
			{
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == OUT))
			{
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == OUT))
			{
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			break;
		case type_difference:
			if ((As == IN) && (Bs == ENTRY))
			{
				thisHit = EXIT;
				theHit.object = Bcit->object;
			}
			if ((As == IN) && (Bs == EXIT))
			{
				thisHit = ENTRY;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == OUT))
			{
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == OUT))
			{
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			break;
		default:
			assert(false);
		}
		if (thisHit != OUT)
		{
			localHits.push_back(theHit);
		}

		if (Ago)
			Acit++;
		else
			Bcit++;

	}

	std::vector<Hit>::const_iterator localCit = localHits.begin();

	while (localCit != localHits.end())
	{
		if (localCit->t > 0)
		{
			hits.push_back(*localCit);
		}
		localCit++;
	}
	return (!hits.empty());
}

// This should never be called.
vec3 Boolean::Normal(const vec3& point) const
{
	assert(false);
	return vec3();
}
