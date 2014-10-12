#ifndef _AWUI_EFFECTS_EFFECT_H
#define _AWUI_EFFECTS_EFFECT_H

#include <awui/Object.h>
#include <awui/String.h>

namespace awui {
	namespace Effects {
		class Effect : public Object {
			private:
				String name;

			public:
				Effect(String name1);

				String GetName();

				virtual float Calculate(float p) const = 0;
		};

		class EffectLinear : public Effect {
			public:
				EffectLinear() : Effect("Linear") {}

				virtual float Calculate(float p) const;
		};

		class EffectSwing : public Effect {
			public:
				EffectSwing() : Effect("Swing") {};

				virtual float Calculate(float p) const;
		};

		class EffectQuad : public Effect {
			public:
				EffectQuad() : Effect("Quad") {};

				virtual float Calculate(float p) const;
		};

		class EffectCubic : public Effect {
			public:
				EffectCubic() : Effect("Cubic") {};

				virtual float Calculate(float p) const;
		};

		class EffectQuart : public Effect {
			public:
				EffectQuart() : Effect("Quart") {};

				virtual float Calculate(float p) const;
		};

		class EffectQuint : public Effect {
			public:
				EffectQuint() : Effect("Quint") {};

				virtual float Calculate(float p) const;
		};

		class EffectExpo : public Effect {
			public:
				EffectExpo() : Effect("Expo") {};

				virtual float Calculate(float p) const;
		};

		class EffectSine : public Effect {
			public:
				EffectSine() : Effect("Sine") {};

				virtual float Calculate(float p) const;
		};

		class EffectCirc : public Effect {
			public:
				EffectCirc() : Effect("Circ") {};

				virtual float Calculate(float p) const;
		};

		class EffectElastic : public Effect {
			public:
				EffectElastic() : Effect("Elastic") {};

				virtual float Calculate(float p) const;
		};

		class EffectBack : public Effect {
			public:
				EffectBack() : Effect("Back") {};

				virtual float Calculate(float p) const;
		};

		class EffectBounce : public Effect {
			public:
				EffectBounce() : Effect("Bounce") {};

				virtual float Calculate(float p) const;
		};

		class EffectIn {
			public:
				float Calculate(float p, Effect *effect) const;
		};

		class EffectOut {
			public:
				float Calculate(float p, Effect *effect) const;
		};

		class EffectInOut {
			public:
				float Calculate(float p, Effect *effect) const;
		};
	}
}

#endif