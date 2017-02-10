#include "boolean.h"

// Determine if the ray intersects with the boolean of A and B.
bool Boolean::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
	// TODO
	//std::cout << "Boolean::Intersection";
	//std::cout << type << std::endl;
	std::vector<Hit> A_hits;
	std::vector<Hit> B_hits;
	//std::cout << "#1";
	A->Intersection(ray, A_hits);
	//std::cout << "#2";
	B->Intersection(ray, B_hits);
	//std::cout << "#3" << std::endl;

	std::vector<Hit>::const_iterator Acit = A_hits.begin();
	std::vector<Hit>::const_iterator Bcit = B_hits.begin();
	double t = 0;
	bool Ago;
	std::vector<Hit> localHits;

	enum STATUS
	{
		IN = 419, OUT, ENTRY, EXIT
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
			//std::cout << "AGo ";
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
			//std::cout << "BGo ";
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
		//std::cout << t;
		switch (type)
		{
		case type_intersection:
			//std::cout << "INTER:";
			if ((As == IN) && (Bs == ENTRY))
			{
				//std::cout << "1";
				thisHit = ENTRY;
				theHit.object = Bcit->object;
			}
			if ((As == IN) && (Bs == EXIT))
			{
				//std::cout << "2";
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == IN))
			{
				//std::cout << "3";
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == IN))
			{
				//std::cout << "4";
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			//std::cout << std::endl;
			break;
		case type_union:
			//std::cout << "UNION:";
			if ((As == OUT) && (Bs == ENTRY))
			{
				//std::cout << "1";
				thisHit = ENTRY;
				theHit.object = Bcit->object;
			}
			if ((As == OUT) && (Bs == EXIT))
			{
				//std::cout << "2";
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == OUT))
			{
				//std::cout << "3";
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == OUT))
			{
				//std::cout << "4";
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			//std::cout << std::endl;
			break;
		case type_difference:
			//std::cout << "DIFF:";
			if ((As == IN) && (Bs == ENTRY))
			{
				//std::cout << "1";
				thisHit = EXIT;
				theHit.object = Bcit->object;
			}
			if ((As == IN) && (Bs == EXIT))
			{
				//std::cout << "2";
				thisHit = ENTRY;
				theHit.ray_exiting = true;
				theHit.object = Bcit->object;
			}
			if ((As == ENTRY) && (Bs == OUT))
			{
				//std::cout << "3";
				thisHit = ENTRY;
				theHit.object = Acit->object;
			}
			if ((As == EXIT) && (Bs == OUT))
			{
				//std::cout << "4";
				thisHit = EXIT;
				theHit.ray_exiting = true;
				theHit.object = Acit->object;
			}
			//std::cout << std::endl;
			break;
		default:
			assert(false);
		}
		if (thisHit != OUT)
		{
			localHits.push_back(theHit);
			//std::cout << "Push:" << theHit.t << std::endl;
		}

		if (Ago)
			Acit++;
		else
			Bcit++;

	}

	std::vector<Hit>::const_iterator localCit = localHits.begin();

	while (localCit != localHits.end())
	{
		if (true)//(localCit->t > 0)
		{
			//std::cout << "RealPush:" << localCit->t << std::endl;
			hits.push_back(*localCit);
		}
		localCit++;
	}
	//std::cout << "Boolean::Intersection RETURN" << !hits.empty() << std::endl;
	return (!hits.empty());
}

// This should never be called.
vec3 Boolean::Normal(const vec3& point) const
{
	assert(false);
	return vec3();
}
