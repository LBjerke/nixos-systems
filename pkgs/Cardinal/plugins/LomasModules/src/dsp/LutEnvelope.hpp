#include "plugin.hpp"

// Envelope shape Look up tables
static const float env_quartic[257] = {0.000000f,0.000000f,0.000000f,0.000000f,0.000001f,0.000002f,0.000004f,0.000007f,0.000010f,0.000015f,0.000021f,0.000029f,0.000039f,0.000051f,0.000065f,0.000082f,0.000102f,0.000125f,0.000151f,0.000180f,0.000214f,0.000251f,0.000293f,0.000340f,0.000391f,0.000448f,0.000511f,0.000579f,0.000653f,0.000734f,0.000821f,0.000915f,0.001017f,0.001127f,0.001244f,0.001370f,0.001504f,0.001647f,0.001800f,0.001962f,0.002134f,0.002316f,0.002509f,0.002713f,0.002928f,0.003155f,0.003393f,0.003645f,0.003908f,0.004185f,0.004476f,0.004780f,0.005098f,0.005431f,0.005779f,0.006142f,0.006520f,0.006915f,0.007326f,0.007754f,0.008199f,0.008661f,0.009142f,0.009640f,0.010158f,0.010694f,0.011251f,0.011826f,0.012423f,0.013040f,0.013678f,0.014337f,0.015019f,0.015722f,0.016449f,0.017198f,0.017972f,0.018769f,0.019590f,0.020437f,0.021308f,0.022205f,0.023129f,0.024078f,0.025055f,0.026059f,0.027091f,0.028151f,0.029239f,0.030357f,0.031504f,0.032682f,0.033889f,0.035128f,0.036397f,0.037699f,0.039033f,0.040399f,0.041798f,0.043231f,0.044698f,0.046199f,0.047735f,0.049307f,0.050914f,0.052558f,0.054238f,0.055955f,0.057710f,0.059504f,0.061335f,0.063206f,0.065116f,0.067067f,0.069057f,0.071089f,0.073162f,0.075277f,0.077435f,0.079635f,0.081878f,0.084165f,0.086497f,0.088873f,0.091295f,0.093762f,0.096276f,0.098836f,0.101443f,0.104098f,0.106802f,0.109554f,0.112355f,0.115206f,0.118107f,0.121058f,0.124061f,0.127116f,0.130222f,0.133382f,0.136594f,0.139860f,0.143181f,0.146556f,0.149986f,0.153472f,0.157014f,0.160613f,0.164269f,0.167983f,0.171755f,0.175586f,0.179476f,0.183427f,0.187437f,0.191508f,0.195641f,0.199836f,0.204093f,0.208413f,0.212797f,0.217244f,0.221757f,0.226334f,0.230977f,0.235686f,0.240461f,0.245304f,0.250215f,0.255194f,0.260242f,0.265359f,0.270546f,0.275803f,0.281132f,0.286532f,0.292004f,0.297549f,0.303166f,0.308858f,0.314624f,0.320464f,0.326380f,0.332372f,0.338440f,0.344585f,0.350808f,0.357109f,0.363488f,0.369947f,0.376486f,0.383105f,0.389804f,0.396586f,0.403449f,0.410394f,0.417423f,0.424536f,0.431733f,0.439014f,0.446381f,0.453835f,0.461374f,0.469001f,0.476715f,0.484517f,0.492409f,0.500389f,0.508460f,0.516621f,0.524874f,0.533217f,0.541654f,0.550183f,0.558805f,0.567521f,0.576332f,0.585239f,0.594240f,0.603339f,0.612534f,0.621826f,0.631217f,0.640706f,0.650294f,0.659983f,0.669771f,0.679661f,0.689652f,0.699746f,0.709942f,0.720242f,0.730645f,0.741154f,0.751767f,0.762486f,0.773311f,0.784244f,0.795284f,0.806432f,0.817689f,0.829055f,0.840531f,0.852117f,0.863815f,0.875625f,0.887547f,0.899581f,0.911730f,0.923992f,0.936370f,0.948863f,0.961471f,0.974197f,0.987040f,1.000000f,1.000000f};
static const float env_linear[257] = {0.000000f,0.003922f,0.007843f,0.011765f,0.015686f,0.019608f,0.023529f,0.027451f,0.031373f,0.035294f,0.039216f,0.043137f,0.047059f,0.050980f,0.054902f,0.058824f,0.062745f,0.066667f,0.070588f,0.074510f,0.078431f,0.082353f,0.086275f,0.090196f,0.094118f,0.098039f,0.101961f,0.105882f,0.109804f,0.113725f,0.117647f,0.121569f,0.125490f,0.129412f,0.133333f,0.137255f,0.141176f,0.145098f,0.149020f,0.152941f,0.156863f,0.160784f,0.164706f,0.168627f,0.172549f,0.176471f,0.180392f,0.184314f,0.188235f,0.192157f,0.196078f,0.200000f,0.203922f,0.207843f,0.211765f,0.215686f,0.219608f,0.223529f,0.227451f,0.231373f,0.235294f,0.239216f,0.243137f,0.247059f,0.250980f,0.254902f,0.258824f,0.262745f,0.266667f,0.270588f,0.274510f,0.278431f,0.282353f,0.286275f,0.290196f,0.294118f,0.298039f,0.301961f,0.305882f,0.309804f,0.313725f,0.317647f,0.321569f,0.325490f,0.329412f,0.333333f,0.337255f,0.341176f,0.345098f,0.349020f,0.352941f,0.356863f,0.360784f,0.364706f,0.368627f,0.372549f,0.376471f,0.380392f,0.384314f,0.388235f,0.392157f,0.396078f,0.400000f,0.403922f,0.407843f,0.411765f,0.415686f,0.419608f,0.423529f,0.427451f,0.431373f,0.435294f,0.439216f,0.443137f,0.447059f,0.450980f,0.454902f,0.458824f,0.462745f,0.466667f,0.470588f,0.474510f,0.478431f,0.482353f,0.486275f,0.490196f,0.494118f,0.498039f,0.501961f,0.505882f,0.509804f,0.513725f,0.517647f,0.521569f,0.525490f,0.529412f,0.533333f,0.537255f,0.541176f,0.545098f,0.549020f,0.552941f,0.556863f,0.560784f,0.564706f,0.568627f,0.572549f,0.576471f,0.580392f,0.584314f,0.588235f,0.592157f,0.596078f,0.600000f,0.603922f,0.607843f,0.611765f,0.615686f,0.619608f,0.623529f,0.627451f,0.631373f,0.635294f,0.639216f,0.643137f,0.647059f,0.650980f,0.654902f,0.658824f,0.662745f,0.666667f,0.670588f,0.674510f,0.678431f,0.682353f,0.686275f,0.690196f,0.694118f,0.698039f,0.701961f,0.705882f,0.709804f,0.713725f,0.717647f,0.721569f,0.725490f,0.729412f,0.733333f,0.737255f,0.741176f,0.745098f,0.749020f,0.752941f,0.756863f,0.760784f,0.764706f,0.768627f,0.772549f,0.776471f,0.780392f,0.784314f,0.788235f,0.792157f,0.796078f,0.800000f,0.803922f,0.807843f,0.811765f,0.815686f,0.819608f,0.823529f,0.827451f,0.831373f,0.835294f,0.839216f,0.843137f,0.847059f,0.850980f,0.854902f,0.858824f,0.862745f,0.866667f,0.870588f,0.874510f,0.878431f,0.882353f,0.886275f,0.890196f,0.894118f,0.898039f,0.901961f,0.905882f,0.909804f,0.913725f,0.917647f,0.921569f,0.925490f,0.929412f,0.933333f,0.937255f,0.941176f,0.945098f,0.949020f,0.952941f,0.956863f,0.960784f,0.964706f,0.968627f,0.972549f,0.976471f,0.980392f,0.984314f,0.988235f,0.992157f,0.996078f,1.000000f,1.000000f};
static const float env_exponential_2[257] = {0.000000f,0.009011f,0.017952f,0.026824f,0.035626f,0.044360f,0.053026f,0.061624f,0.070156f,0.078621f,0.087020f,0.095354f,0.103624f,0.111828f,0.119969f,0.128047f,0.136061f,0.144014f,0.151904f,0.159733f,0.167501f,0.175209f,0.182857f,0.190445f,0.197974f,0.205444f,0.212857f,0.220211f,0.227509f,0.234750f,0.241934f,0.249062f,0.256135f,0.263153f,0.270116f,0.277025f,0.283881f,0.290683f,0.297432f,0.304128f,0.310773f,0.317365f,0.323907f,0.330397f,0.336837f,0.343227f,0.349567f,0.355858f,0.362100f,0.368293f,0.374438f,0.380536f,0.386585f,0.392588f,0.398544f,0.404454f,0.410317f,0.416135f,0.421908f,0.427636f,0.433319f,0.438958f,0.444554f,0.450105f,0.455614f,0.461079f,0.466502f,0.471883f,0.477222f,0.482519f,0.487775f,0.492991f,0.498165f,0.503300f,0.508394f,0.513449f,0.518464f,0.523441f,0.528379f,0.533278f,0.538139f,0.542962f,0.547748f,0.552497f,0.557208f,0.561883f,0.566522f,0.571124f,0.575691f,0.580222f,0.584718f,0.589179f,0.593605f,0.597996f,0.602354f,0.606677f,0.610967f,0.615224f,0.619447f,0.623638f,0.627796f,0.631922f,0.636015f,0.640077f,0.644107f,0.648105f,0.652073f,0.656010f,0.659916f,0.663791f,0.667637f,0.671452f,0.675238f,0.678995f,0.682722f,0.686420f,0.690089f,0.693730f,0.697343f,0.700927f,0.704484f,0.708012f,0.711514f,0.714988f,0.718435f,0.721855f,0.725249f,0.728616f,0.731957f,0.735272f,0.738561f,0.741825f,0.745063f,0.748276f,0.751464f,0.754627f,0.757766f,0.760880f,0.763970f,0.767036f,0.770078f,0.773096f,0.776091f,0.779063f,0.782011f,0.784937f,0.787840f,0.790720f,0.793577f,0.796413f,0.799226f,0.802018f,0.804788f,0.807536f,0.810263f,0.812968f,0.815653f,0.818317f,0.820960f,0.823582f,0.826184f,0.828766f,0.831327f,0.833869f,0.836391f,0.838893f,0.841376f,0.843840f,0.846284f,0.848709f,0.851116f,0.853504f,0.855873f,0.858223f,0.860556f,0.862870f,0.865166f,0.867445f,0.869705f,0.871948f,0.874174f,0.876382f,0.878573f,0.880747f,0.882904f,0.885045f,0.887168f,0.889276f,0.891366f,0.893441f,0.895499f,0.897542f,0.899568f,0.901579f,0.903574f,0.905553f,0.907517f,0.909466f,0.911400f,0.913318f,0.915222f,0.917111f,0.918985f,0.920844f,0.922690f,0.924520f,0.926337f,0.928139f,0.929927f,0.931702f,0.933462f,0.935209f,0.936943f,0.938662f,0.940369f,0.942062f,0.943742f,0.945409f,0.947063f,0.948704f,0.950332f,0.951948f,0.953551f,0.955141f,0.956719f,0.958285f,0.959839f,0.961381f,0.962910f,0.964428f,0.965934f,0.967428f,0.968911f,0.970382f,0.971841f,0.973290f,0.974727f,0.976152f,0.977567f,0.978971f,0.980363f,0.981745f,0.983116f,0.984477f,0.985827f,0.987166f,0.988495f,0.989814f,0.991122f,0.992420f,0.993708f,0.994986f,0.996255f,0.997513f,0.998761f,1.000000f,1.000000f};
static const float env_exponential_8[257] = {0.000000f,0.030777f,0.060608f,0.089521f,0.117544f,0.144705f,0.171030f,0.196545f,0.221276f,0.245245f,0.268477f,0.290995f,0.312819f,0.333972f,0.354474f,0.374346f,0.393606f,0.412273f,0.430366f,0.447903f,0.464899f,0.481373f,0.497341f,0.512816f,0.527816f,0.542354f,0.556445f,0.570103f,0.583340f,0.596170f,0.608605f,0.620658f,0.632339f,0.643662f,0.654636f,0.665272f,0.675581f,0.685573f,0.695258f,0.704644f,0.713742f,0.722560f,0.731107f,0.739390f,0.747419f,0.755201f,0.762743f,0.770053f,0.777139f,0.784006f,0.790662f,0.797114f,0.803366f,0.809427f,0.815301f,0.820994f,0.826512f,0.831860f,0.837044f,0.842068f,0.846938f,0.851658f,0.856233f,0.860667f,0.864964f,0.869129f,0.873166f,0.877079f,0.880872f,0.884548f,0.888110f,0.891564f,0.894911f,0.898154f,0.901299f,0.904346f,0.907300f,0.910162f,0.912937f,0.915626f,0.918233f,0.920759f,0.923208f,0.925581f,0.927881f,0.930111f,0.932272f,0.934366f,0.936396f,0.938364f,0.940271f,0.942119f,0.943911f,0.945647f,0.947330f,0.948961f,0.950542f,0.952074f,0.953559f,0.954999f,0.956394f,0.957746f,0.959057f,0.960327f,0.961559f,0.962752f,0.963909f,0.965030f,0.966116f,0.967169f,0.968190f,0.969179f,0.970138f,0.971068f,0.971969f,0.972842f,0.973688f,0.974508f,0.975303f,0.976074f,0.976820f,0.977544f,0.978246f,0.978926f,0.979585f,0.980223f,0.980843f,0.981443f,0.982024f,0.982588f,0.983134f,0.983664f,0.984177f,0.984675f,0.985157f,0.985624f,0.986077f,0.986516f,0.986942f,0.987354f,0.987754f,0.988141f,0.988517f,0.988881f,0.989233f,0.989575f,0.989907f,0.990228f,0.990539f,0.990841f,0.991133f,0.991417f,0.991692f,0.991958f,0.992216f,0.992466f,0.992708f,0.992943f,0.993171f,0.993392f,0.993606f,0.993813f,0.994014f,0.994209f,0.994398f,0.994581f,0.994758f,0.994930f,0.995097f,0.995258f,0.995415f,0.995567f,0.995714f,0.995856f,0.995994f,0.996128f,0.996258f,0.996384f,0.996506f,0.996624f,0.996738f,0.996849f,0.996957f,0.997061f,0.997162f,0.997260f,0.997355f,0.997447f,0.997536f,0.997623f,0.997707f,0.997788f,0.997867f,0.997943f,0.998017f,0.998089f,0.998158f,0.998225f,0.998291f,0.998354f,0.998415f,0.998475f,0.998532f,0.998588f,0.998642f,0.998694f,0.998745f,0.998795f,0.998842f,0.998889f,0.998933f,0.998977f,0.999019f,0.999060f,0.999099f,0.999138f,0.999175f,0.999211f,0.999246f,0.999280f,0.999313f,0.999344f,0.999375f,0.999405f,0.999434f,0.999462f,0.999489f,0.999516f,0.999541f,0.999566f,0.999590f,0.999613f,0.999636f,0.999658f,0.999679f,0.999699f,0.999719f,0.999739f,0.999757f,0.999775f,0.999793f,0.999810f,0.999826f,0.999842f,0.999858f,0.999873f,0.999888f,0.999902f,0.999915f,0.999929f,0.999941f,0.999954f,0.999966f,0.999978f,0.999989f,1.000000f,1.000000f};
static float const* const env_shapes_[4] = {env_quartic, env_linear, env_exponential_2, env_exponential_8};

struct LutEnvelope
{
    float process(float delta_time)
    {
        // Instant increment.
        if (increment_[stage_] > 99999) {
            value_ = target_level_[0];
            setStage(stage_ + 1);
        }

        // Increment phase.
        phase_ += increment_[stage_] * delta_time;

        // Stage change.
        if (phase_ > 1.0f)
            setStage(stage_ + 1);

        // Interpolate between two envelope shapes. Quartic, Linear, Exponential.
        EnvShape shape_a = (EnvShape)shape_[stage_];
        EnvShape shape_b = (EnvShape)clamp(shape_[stage_] + 1, 0.f, (int)NUM_ENV_SHAPES - 1.f);

        float position = crossfade(
                             interpolateLinear(env_shapes_[shape_a], phase_ * 255),
                             interpolateLinear(env_shapes_[shape_b], phase_ * 255),
                             fmod(shape_[stage_], 1.f));

        value_ = start_value_ + (target_level_[stage_] - start_value_) * position;

        return value_;
    }

    void tigger(bool reset)
    {
        if (reset) 
            value_ = 0; //  # for mono-style envelopes that are reset to 0 on new notes
        
        setStage(0);
    }

    void configureStage(int stage, float target_level, float increment, float shape)
    {
        target_level_[stage] = target_level;
        increment_[stage] = increment;
        shape_[stage] = shape;
    }

    void envelopeAD(float attack, float decay, float attack_shape = 1.85f, float decay_shape = 2.99999f)
    {
        //           Stage  Level  Increment                                    Shape
        configureStage(0,   1.0f,  interpolateLinear(env_time_, attack * 255),  attack_shape);// Attack Linear / Exponential
        configureStage(1,   0.0f,  interpolateLinear(env_time_, decay  * 255),  decay_shape); // Decay  Exponential
        configureStage(2,   0.0f,  0.0f,                                        1.0f);        // Off    Linear
    }

    void envelopeHD(float hold, float decay, float decay_shape = 2.99999f)
    {
        //           Stage  Level  Increment                                   Shape
        configureStage(0,   1.0f,  4000,                                       1.0f);     // Attack Linear
        configureStage(1,   1.0f,  interpolateLinear(env_time_, hold * 255),   1.0f);     // Hold   Linear
        configureStage(2,   0.0f,  interpolateLinear(env_time_, decay * 255),  decay_shape); // Decay  Exponential
        configureStage(3,   0.0f,  0.0f,                                       1.0f);     // Off    Linear
    }

    // To set a segment time with a 0 to 1 input the envelope has to calculate
    // pow(max_time / min_time, segment_time) / min_time wich is expensive.
    // Set min and max time here updates the lookuptable.
    void setMinMaxTime(float min_time, float max_time)
    {
        // Update ToEnvTime lookup.
        for (size_t i = 0; i < 256; i++) {
            float t = (1.f/256.f) * i;
            env_time_[i] = ParamToEnvTime(t, min_time, max_time);
        }
    }

    // Converts from 0 to 1 to envelope increments.
    float ParamToEnvTime(float time, float min_time, float max_time) {
        return pow(max_time / min_time, -time) / min_time;
    }

    enum EnvShape { QUARTIC, LINEAR, EXPONENTIAL, EXPONENTIAL_8, NUM_ENV_SHAPES };

private:

    int stage_ = 0;
    float phase_ = 0;
    float value_ = 0;
    float start_value_ = 0;
    float target_level_[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    float increment_[4]    = {1.0f, 1.0f, 1.0f, 0.0f};
    float shape_[4]        = {1.0f, 1.0f, 1.0f, 0.0f};

    // Envelope time from 1ms to 2 min. Middle is 1 seg. Last is infinite
    float env_time_[257] = {1000.000000f,947.061000f,896.924538f,849.442250f,804.473627f,761.885597f,721.552136f,683.353887f,647.177816f,612.916870f,580.469663f,549.740180f,520.637485f,493.075457f,466.972535f,442.251476f,418.839125f,396.666201f,375.667089f,355.779649f,336.945030f,319.107497f,302.214266f,286.215345f,271.063390f,256.713566f,243.123406f,230.252696f,218.063349f,206.519293f,195.586368f,185.232222f,175.426213f,166.139325f,157.344075f,149.014437f,141.125762f,133.654705f,126.579159f,119.878185f,113.531953f,107.521685f,101.829595f,96.438838f,91.333462f,86.498360f,81.919223f,77.582502f,73.475362f,69.585649f,65.901855f,62.413076f,59.108991f,55.979820f,53.016304f,50.209674f,47.551624f,45.034289f,42.650218f,40.392359f,38.254027f,36.228898f,34.310976f,32.494587f,30.774356f,29.145193f,27.602275f,26.141038f,24.757158f,23.446539f,22.205302f,21.029776f,19.916481f,18.862122f,17.863580f,16.917900f,16.022283f,15.174080f,14.370779f,13.610004f,12.889504f,12.207147f,11.560913f,10.948890f,10.369266f,9.820328f,9.300449f,8.808093f,8.341801f,7.900195f,7.481966f,7.085878f,6.710759f,6.355498f,6.019045f,5.700402f,5.398629f,5.112831f,4.842163f,4.585823f,4.343054f,4.113138f,3.895392f,3.689174f,3.493873f,3.308911f,3.133740f,2.967843f,2.810729f,2.661931f,2.521011f,2.387552f,2.261157f,2.141454f,2.028087f,1.920722f,1.819041f,1.722743f,1.631543f,1.545170f,1.463371f,1.385901f,1.312533f,1.243049f,1.177243f,1.114921f,1.055898f,1.000000f,1.000000f,0.957763f,0.917310f,0.878566f,0.841459f,0.805918f,0.771879f,0.739277f,0.708052f,0.678147f,0.649504f,0.622071f,0.595797f,0.570632f,0.546531f,0.523447f,0.501338f,0.480163f,0.459883f,0.440459f,0.421855f,0.404038f,0.386972f,0.370628f,0.354974f,0.339981f,0.325621f,0.311868f,0.298696f,0.286080f,0.273997f,0.262424f,0.251340f,0.240724f,0.230557f,0.220819f,0.211492f,0.202560f,0.194004f,0.185810f,0.177962f,0.170445f,0.163246f,0.156351f,0.149748f,0.143423f,0.137365f,0.131563f,0.126006f,0.120684f,0.115587f,0.110705f,0.106029f,0.101551f,0.097262f,0.093154f,0.089219f,0.085451f,0.081842f,0.078385f,0.075074f,0.071903f,0.068866f,0.065958f,0.063172f,0.060504f,0.057948f,0.055501f,0.053156f,0.050911f,0.048761f,0.046701f,0.044729f,0.042840f,0.041030f,0.039297f,0.037638f,0.036048f,0.034525f,0.033067f,0.031670f,0.030333f,0.029052f,0.027825f,0.026649f,0.025524f,0.024446f,0.023413f,0.022424f,0.021477f,0.020570f,0.019701f,0.018869f,0.018072f,0.017309f,0.016578f,0.015878f,0.015207f,0.014565f,0.013950f,0.013360f,0.012796f,0.012256f,0.011738f,0.011242f,0.010767f,0.010313f,0.009877f,0.009460f,0.009060f,0.008678f,0.008311f,0.007960f,0.007624f,0.007302f,0.006993f,0.006698f,0.006415f,0.006144f,0.005885f,0.005636f,0.005398f,0.005170f,0.004952f,0.004743f,0.004542f,0.004350f, 0.0f, 0.0f};//0.004350f, 0.004167f};

    void setStage(int newStage)
    {
        phase_ = 0.f;
        stage_ = newStage;
        start_value_ = value_;
    }
};

