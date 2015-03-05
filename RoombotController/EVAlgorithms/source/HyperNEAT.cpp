#include "HyperNEAT.h"

const std::string HyperNEAT::LAYER_INPUT_NAME = "InputLayer";
const std::string HyperNEAT::LAYER_HIDDEN_NAME = "ProcessingLayer";
const std::string HyperNEAT::LAYER_OUTPUT_NAME = "OutputLayer";

HyperNEAT::HyperNEAT(unsigned int seed,
					 const std::string & parametersPath,                // Path to the parameter file for HyperNEAT
					 const std::string & logDirectory,                  // Logging Directory
					 const Vector3<double> & organismSize,         // The bounding box of the organism
					 const std::vector<Vector3<double> > & indexes, // The indices of the modules int he organism
                     const int numMotors,                               // The number of motors per module
					 std::size_t extraParameters)                         // Parameter size is the number of extra parameters
:
LearningAlgorithm::LearningAlgorithm("HyperNEAT"),
_globals(parametersPath.empty() ? NEAT::Globals::init() : NEAT::Globals::init(parametersPath)),
_directory(logDirectory),
_i_index_last(0),
_g_index_prev(0),
_g_index_last(0)
{

	/*	unsigned int seed;
		int index_1, index_2, count;

		boost::filesystem::path _log_path;

		boost::random::random_device rng;
		boost::random::uniform_int_distribution<unsigned int> dist(0, UINT_MAX);

		_directory /= _name;

		if (!boost::filesystem::exists(_directory)) {
			boost::filesystem::create_directory(_directory);
		}

		seed = dist(rng);
		_directory /= boost::lexical_cast<std::string>(seed);

		if (!boost::filesystem::exists(_directory)) {
			boost::filesystem::create_directory(_directory);
		}


	 */

	_globals->seedRandom(seed);

    std::cout << "Constructor parameter size: " << extraParameters << " number of motors: " << numMotors << std::endl;
    
	std::size_t hidden_layers(static_cast<std::size_t> (_globals->getParameterValue("HiddenLayers")));
	std::size_t population_size(static_cast<std::size_t> (_globals->getParameterValue("PopulationSize")));

	JGTL::Vector2<int> motorLayerDimensions(initMotorLayerDimensions(organismSize,numMotors));
	JGTL::Vector2<int> parameterDimensions(initParameterLayerDimensions(extraParameters, motorLayerDimensions));
	JGTL::Vector2<int> totalLayerDimensions(motorLayerDimensions.x + parameterDimensions.x, motorLayerDimensions.y);

    std::cout << "Total calculated layer dimensions: " << totalLayerDimensions.x << ", " << totalLayerDimensions.y << std::endl;
    
	SubstrateInfo substrateInfo(initialiseSubstrateInfo(hidden_layers, totalLayerDimensions));
	Genome genome(_init_genome(hidden_layers));

	_population = _init_population(population_size, genome);
	_substrate.setLayerInfo(substrateInfo);

	/* These should be always 0 and # Hidden Layers + 1 */
	int z_input = _substrate.getLayerIndex(HyperNEAT::LAYER_INPUT_NAME);
	int z_output = _substrate.getLayerIndex(HyperNEAT::LAYER_OUTPUT_NAME);

	_mappings_in = _init_mappings(indexes, motorLayerDimensions, numMotors, extraParameters, parameterDimensions);
	_mappings_out = _mappings_in;

    
	for (std::size_t index = 0; index < _mappings_in.size(); index++) {
		_mappings_in[index].z = z_input;
		_mappings_out[index].z = z_output;
	}

	_substrate.populateSubstrate(_population.getIndividual(_i_index_last, _g_index_last));

	if (!boost::filesystem::exists(_directory) || !boost::filesystem::is_directory(_directory)) {
		boost::filesystem::create_directories(_directory);
	}

	_log_i.open((_directory / "Log_Fitness_Individuals.txt").c_str(), std::ofstream::app);
	_log_c.open((_directory / "Log_Fitness_Champions.txt").c_str(), std::ofstream::app);
	_log_s.open((_directory / "Log_Substrates_Champions.txt").c_str(), std::ofstream::app);
}

HyperNEAT::~HyperNEAT()
{
	save();
}

void HyperNEAT::setParameter(std::size_t index, double value)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Parameter" << " "
			<< index << " "
			<< "-" << " "
			<< _mappings_in[index] << " "
			<< value
			<< std::endl;
#endif

	_substrate.setValue(_mappings_in[index], value);
}

double HyperNEAT::getParameter(std::size_t index)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Get Parameter" << " "
			<< index << " "
			<< "-" << " "
			<< _mappings_out[index] << " "
			<< _substrate.getValue(_mappings_out[index])
			<< std::endl;
#endif

	return _substrate.getValue(_mappings_out[index]);
}

void HyperNEAT::reset()
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Reset Parameters"
			<< std::endl;
#endif

	_substrate.getNetwork()->reinitialize();
}

void HyperNEAT::update()
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Update Parameters"
			<< std::endl;
#endif

	_substrate.getNetwork()->dummyActivation();
	_substrate.getNetwork()->update();
}

unsigned int HyperNEAT::getGeneration() {
    return _g_index_last;
}

unsigned int HyperNEAT::getEvaluation() {
    return _i_index_last;
}

void HyperNEAT::setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize) {
    
}

boost::ptr_vector<MindGenome> HyperNEAT::getRandomInitialMinds() {
    boost::ptr_vector<MindGenome> minds;
    
    return minds;
}

bool HyperNEAT::nextEvaluation()
{
	int d_index, d_modulo, next_index;
	boost::shared_ptr<Individual> individual = _population.getIndividual(_i_index_last, _g_index_last);

	_log_i
			<< _g_index_last << " "
			<< _i_index_last << " "
			<< individual->getUserData() <<
			std::endl;

	_i_index_last += 1;

	if (_population.getIndividualCount(_g_index_last) == _i_index_last) {
		individual = _population.getBestIndividualOfGeneration();

		_log_c
				<< _g_index_last << " "
				<< individual->getUserData() <<
				std::endl;

		_log_s
				<< "Individual:" << " "
				<< _g_index_last << " "
				<< individual->getUserData() <<
				std::endl;

		_substrate.populateSubstrate(individual);
		_substrate.dump(_log_s);

		_population.adjustFitness();
		_population.produceNextGeneration();

		next_index = _g_index_last + 1;
		d_modulo = _globals->getParameterValue("GenerationDumpModulo");

        // Log the previous generation
		if (!(next_index % d_modulo)) {
			for (d_index = (next_index - d_modulo); d_index < next_index; d_index++) {
				save_individual(d_index + _g_index_prev, _population.getBestIndividualOfGeneration(d_index), _directory.string());
				save_generation(d_index + _g_index_prev, _population.getGeneration(d_index), _directory.string());
			}

			_population.cleanupOld(next_index);
		}

		_g_index_last = next_index;
		_i_index_last = 0;
	}

#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Getting Generation#"
			<< _g_index_last
			<< " Individual # "
			<< _i_index_last
			<< std::endl;
#endif

	if (_g_index_last == static_cast<int> (_globals->getParameterValue("MaxGenerations"))) {
		return false;
	}

	individual = _population.getIndividual(_i_index_last, _g_index_last);
	_substrate.populateSubstrate(individual);

	return true;
}

void HyperNEAT::setEvaluationFitness(double value)
{
	boost::shared_ptr<Individual> individual = _population.getIndividual(_i_index_last, _g_index_last);

#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Fitness" << " "
			<< "Generation" << " "
			<< _g_index_last << " "
			<< "Individual" << " "
			<< _i_index_last << " "
			<< value
			<< std::endl;
#endif

	individual->setFitness(value);
}

void HyperNEAT::setEvaluationFitnessAlt(const std::string & string)
{
	boost::shared_ptr<Individual> individual = _population.getIndividual(_i_index_last, _g_index_last);

#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Fitness Alternate" << " "
			<< "Generation" << " "
			<< _g_index_last << " "
			<< "Individual" << " "
			<< _i_index_last << " "
			<< string
			<< std::endl;
#endif

	individual->setUserData(string);
}

void HyperNEAT::save()
{
	int d_index, d_modulo, d_remainder;
	//TiXmlDocument document(path);

	//document.LinkEndChild(get_state());

	d_modulo = _globals->getParameterValue("GenerationDumpModulo");
	d_remainder = _g_index_last % d_modulo;

	if (d_remainder) {
		for (d_index = (_g_index_last - d_remainder); d_index < _g_index_last; d_index++) {
			save_individual(d_index + _g_index_prev, _population.getBestIndividualOfGeneration(d_index), _directory.string());
			save_generation(d_index + _g_index_prev, _population.getGeneration(d_index), _directory.string());
		}
	}

	_population.cleanupOld(_g_index_last - 1);

	_log_i.close();
	_log_c.close();
	_log_s.close();
	/*_population.dump(, true, false);
	element-
	element->SetAttribute("LastIndividualID", _i_index_last - 1);
	element->SetAttribute("LastGenerationID", _g_index_last - 1);
	document.LinkEndChild(element);

	document.SaveFile();*/
}

// </editor-fold>

//TiXmlElement * HyperNEAT::get_state() {
//	/*TiXmlElement * state = new TiXmlElement("HyperNEATState");
//	TiXmlElement * o_info = new TiXmlElement("OtherInfo");
//
//
//	o_info->SetAttribute("Directory", _directory.string());
//	o_info->SetAttribute("Flag", );
//	o_info->SetAttribute();*/
//}

void HyperNEAT::save_individual(const int & index, const boost::shared_ptr<NEAT::GeneticIndividual> & individual, const std::string & path)
{
	boost::filesystem::path logDirectory(path);

	logDirectory /= ("Generation_" + boost::lexical_cast<std::string>(index) + "_Best_Individual.xml");

	TiXmlDocument document(logDirectory.string());
	TiXmlElement * element = new TiXmlElement("Individual");

	individual->dump(element, true);
	element->SetAttribute("GenerationID", index);
	document.LinkEndChild(element);

	document.SaveFile();
#ifdef DEBUG_ALGORITHM
	std::cout << "Saved Generation " << index << " Best Individual" << std::endl;
#endif
}

void HyperNEAT::save_generation(const int & index, const boost::shared_ptr<NEAT::GeneticGeneration> & generation, const std::string & path)
{
	boost::filesystem::path logDirectory(path);

	logDirectory /= ("Generation_" + boost::lexical_cast<std::string>(index) + ".xml");

	TiXmlDocument document(logDirectory.string());
	TiXmlElement * element = new TiXmlElement("Generation");

	generation->dump(element, true);
	document.LinkEndChild(element);

	document.SaveFile();
#ifdef DEBUG_ALGORITHM
	std::cout << "Saved Generation: " << index << std::endl;
#endif
}

JGTL::Vector2<int> HyperNEAT::initMotorLayerDimensions(const Vector3<double> & organismSize, const int numMotors)
{
	int organismXSize = ((static_cast<int> (organismSize.x) * 2) - 1) * numMotors;
	int organismYSize = (static_cast<int> (organismSize.y) * 2) - 1;
	int organismZSize = (static_cast<int> (organismSize.z) * 2) - 1;

    
    std::cout << "Initializing motor layer dimension: x: " << organismXSize << ", y: " << organismYSize << ", z: " << organismZSize << std::endl;
    
	if (1 < organismZSize) {
		throw std::invalid_argument("Only Planar Shapes Supported");
	}

	return JGTL::Vector2<int>(organismXSize, organismYSize);
}

/**
 * Calculates the size of the substrate layer needed to address the robot
 * parameters taking into account the number of parameters and the size
 * of the substrate used to address the robot servos
 *
 * @param Parameters size
 * @param Robot substrate sizes
 * @return Sizes of the parameters substrate layer
 */
JGTL::Vector2<int> HyperNEAT::initParameterLayerDimensions(std::size_t parameterSize, const JGTL::Vector2<int> & organismLayerDimension)
{

    std::cout << "Parameter size: " << parameterSize << " organism layer y: " << organismLayerDimension.y << " calculation: " << (double)parameterSize/(double)organismLayerDimension.y << " ceiling: "  << std::ceil((double)parameterSize/(double)organismLayerDimension.y) << std::endl;
    int parameterXSize = std::ceil((double)parameterSize/(double)organismLayerDimension.y);
	int parameterYSize = organismLayerDimension.y;
    
    std::cout << "Initializing parameter layer dimension: x: " << parameterXSize << ", y: " << parameterYSize << std::endl;
    
    return JGTL::Vector2<int>(parameterXSize, parameterYSize);
}

/**
 * Sets up information about each substrate layer, starting from the input
 * layer, then the hidden layers and at last the output layer
 * We use the same size for each layer, the names for each layer are defined
 * in the static variables of this class
 *
 * @param Hidden layers
 * @param Layer size
 * @return Substrate information
 */
HyperNEAT::SubstrateInfo HyperNEAT::initialiseSubstrateInfo(std::size_t numHiddenLayers, const JGTL::Vector2<int> & layerDimensions)
{
	if (JGTL::Vector2<int>(0, 0) == layerDimensions) {
		throw std::invalid_argument("Invalid Layer Size");
	}

	JGTL::Vector3<float> coords(0.0, 0.0, 0.0), delta(0.0, 5.0, 0.0);
	SubstrateInfo result;

	/* Layers Info Setup*/

	/* Input Layer */

	result.layerSizes.push_back(layerDimensions);
	result.layerIsInput.push_back(true);
	result.layerLocations.push_back(coords);
	result.layerNames.push_back(HyperNEAT::LAYER_INPUT_NAME);

	coords += delta;

	/* Hidden Layers */

	for (std::size_t index = 0; index < numHiddenLayers; index++) {
		result.layerSizes.push_back(layerDimensions);
		result.layerIsInput.push_back(false);
		result.layerLocations.push_back(coords);
		result.layerNames.push_back(HyperNEAT::LAYER_HIDDEN_NAME + boost::lexical_cast<std::string>(index));

		coords += delta;
	}

	/* Output Layer */

	result.layerSizes.push_back(layerDimensions);
	result.layerIsInput.push_back(false);
	result.layerLocations.push_back(coords);
	result.layerNames.push_back(HyperNEAT::LAYER_OUTPUT_NAME);

	/* Substrate Layers Adjacency Setup */

	std::string l_parent = HyperNEAT::LAYER_INPUT_NAME;

	for (std::size_t index = 0; index < numHiddenLayers; index++) {
		std::string l_child = HyperNEAT::LAYER_HIDDEN_NAME + boost::lexical_cast<std::string>(index);

		result.layerAdjacencyList.push_back(std::make_pair(l_parent, l_child));

		l_parent = l_child;
	}

	result.layerAdjacencyList.push_back(std::make_pair(l_parent, HyperNEAT::LAYER_OUTPUT_NAME));

	// PUT_TRUE_IF_ALL_INCOMING_LINK_WEIGHTS_ARE_NORMALIZED
	result.normalize = true;

	result.useOldOutputNames = false;

	// THIS_CAN_BE_USED_FOR_DISCRETE_SUBSTRATE_EXTRAPOLATION
	result.layerValidSizes = result.layerSizes;

	return result;
}

HyperNEAT::Genome HyperNEAT::_init_genome(std::size_t h_size)
{
	Genome result;

	/*
	 * These are the input node genes used to query the CPPN about substrate
	 * links weights
	 */
	result.push_back(NodeGene("Bias", "NetworkSensor",0, 0, false));
	result.push_back(NodeGene("X1", "NetworkSensor",0, 0, false));
	result.push_back(NodeGene("Y1", "NetworkSensor",0, 0, false));
	result.push_back(NodeGene("X2", "NetworkSensor",0, 0, false));
	result.push_back(NodeGene("Y2", "NetworkSensor",0, 0, false));

#if DEBUG_USE_DELTAS
	result.push_back(NodeGene("DeltaX", "NetworkSensor",0, 0, false));
	result.push_back(NodeGene("DeltaY", "NetworkSensor",0, 0, false));
#endif

	/*
	 * For adjacent substrate layers A and B we create a node gene which outputs link
	 * weights between nodes of layers A and B
	 * The node shall be named like Output_LayerAName_LayerBName
	 */
	std::string l_parent = HyperNEAT::LAYER_INPUT_NAME, name;

	for (std::size_t index = 0; index < h_size; index++) {
		std::string l_child = HyperNEAT::LAYER_HIDDEN_NAME + boost::lexical_cast<std::string>(index);

		name = "Output_" + l_parent + "_" + l_child;

		result.push_back(NodeGene(name, "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));

		l_parent = l_child;
	}

	name = "Output_" + l_parent + "_" + HyperNEAT::LAYER_OUTPUT_NAME;

	result.push_back(NodeGene(name, "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));

#if DEBUG_DIRECT_LINKS
	result.push_back(NodeGene("Output_ac", "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));
#endif
#if DEBUG_ENABLE_BIASES
	result.push_back(NodeGene("Bias_b", "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));
	result.push_back(NodeGene("Bias_c", "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));
#endif

	return result;
}

HyperNEAT::Population HyperNEAT::_init_population(std::size_t size, const Genome & genome)
{
	Population result;

	for (std::size_t index = 0; index < size; index++) {
		boost::shared_ptr<Individual> individual(new Individual(genome, true, 1.0));

		result.addIndividual(individual);
	}

	return result;
}

/* Fix This */
std::vector< JGTL::Vector3<int> > HyperNEAT::_init_mappings(const std::vector<Vector3<double> > & indexes, const JGTL::Vector2<int> & motorLayerDimensions, const int numMotors, std::size_t parameterSize, const JGTL::Vector2<int> & parameterLayerDimensions)
{
	std::size_t organismSize = indexes.size();
    std::size_t totalSize = organismSize*numMotors + parameterSize;

    // Center the organism nodes in the middle of the substrate
	int organismXOffset = (motorLayerDimensions.x/numMotors) / 2;
	int organismYOffset = motorLayerDimensions.y / 2;
    
    std::cout << "";

	std::vector< JGTL::Vector3<int> > result(totalSize);

	for (std::size_t index = 0; index < organismSize; index++) {
        int moduleX = static_cast<int>((indexes[index].x)*-1) + organismXOffset;
        for(int i = 0; i < numMotors; i++){
            result[(index*numMotors)+i].x = (moduleX*numMotors) + i;
            result[(index*numMotors)+i].y = static_cast<int> ((indexes[index].y)*-1) + organismYOffset;

            std::cout << indexes[index] << " -> " << result[(index*numMotors)+i] << ";";
        }
	}

	std::cout << std::endl;

	for (std::size_t index = 0; index < parameterSize; index++) {
		std::div_t division = std::div(index, parameterLayerDimensions.x);

		division.quot = (division.quot % parameterLayerDimensions.y);
        
//        std::cout << "division.rem: " << division.rem << " motorlayer x: " << motorLayerDimensions.x << " added: " << division.rem + motorLayerDimensions.x << " position: " << organismSize*numMotors << std::endl;

		result[index + organismSize*numMotors].x = division.rem + motorLayerDimensions.x;
		result[index + organismSize*numMotors].y = division.quot;

		std::cout << result[index + organismSize*numMotors] << ";";
	}

	std::cout << std::endl;

	return result;

}
