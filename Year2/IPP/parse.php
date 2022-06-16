<?php

/**
 * @file parse.php
 * @brief Parses input code (IPPcode21), checks it's lexical and syntactic correctness and transforms it to output xml format  
 * @author Marek Miček, xmicek08
 * @date 21.2.2021
 */

 ini_set('display_errors', 'stderr');

 /**
  * @class GlobalStuff
  * @brief Provides interface for the opening of STDIN 
  */
 class GlobalStuff {

    public static $stdin;

    public static function openFile(){

        if (!(GlobalStuff::$stdin = fopen('php://stdin', 'r')))
        {    
            Errors::putError(Errors::$fileError, 11);
        }
    }
 }

 /**
  * @class Errors
  * @brief Provides interface for notification of errors and returnig error codes 
  */
 class Errors {

    public static $fileError = "Failed to open stdin !!!\n";
    public static $ArgError = "Forrbidden usage of arguments !!!\n";
    public static $headError = "Header IPPcode21 error !!!\n";
    public static $syntaxError = "Syntax or lexical error !!!\n";
    public static $outfileError = "Failed to write stats to output file !!!\n";

 /**
  * @brief Informs user about type of error, closes input file, exits program
  * @param $errorMessage Specific error message in case of error
  * @param $errorCode Specific error code in case of error  
  */
    public static function putError($errorMessage, $errorCode)
    {
        fwrite(STDERR, $errorMessage);
        fclose(GlobalStuff::$stdin);
        exit($errorCode);
    }
 }

 /** 
  * @class ArgumentParser
  * @brief Provides interface for parsing of command line
  */
 class ArgumentParser {

    public $fileNames = [];     //< stores all output files for stats, to check file names not same 
    public $actFile;            //< name of current file where stats will be written
    public $statSet = FALSE;
    public $firstWrite = TRUE;

    /**
     * @brief Checks for --help argument, all arguments are parsed after whole file is read
     *        but --help is checked now, in case there is no input file, so we can write hint immediately
     */
    public function checkHelp() {

        global $argc;
        if ($argc != 1)
        {
            $opts = getopt("", ["help"]);
            if (array_key_exists('help', $opts)) {

                if ($argc == 2) {
                    $this->printHelp();
                    exit(0);
                }
                else {
                    Errors::putError(Errors::$ArgError, 10);
                }
            }
        }
    }

    /**
     * @brief Parses arguments of commnad line
     * @param $insCounter Stores number of instructions in input file
     *        is used in case user entered param --loc
     */
    public function getArguments($insCounter) {

        global $argc;
        global $argv;
        if ($argc != 1) 
        {
            for ($i = 1; $i < $argc; $i++) {
                if ($argv[$i] == "--help") {
                    if ($argc == 2)
                    {
                        $this->printHelp();
                        exit(0);
                    }
                    else {
                        Errors::putError(Errors::$ArgError, 10);
                    }       
                }
                if (preg_match('/^(--stats=).+$/', $argv[$i])) {
                    $fileName = explode('=', $argv[$i]);
                    $this->actFile = $fileName[1];
                    $this->firstWrite = TRUE;               //< avoid append when first write to existing file
                    file_put_contents($fileName[1], "");    //< in case only --stats will be given
                    
                    if (!$this->statSet) {
                        $this->statSet = TRUE;
                        array_push($this->fileNames, $fileName[1]);   //< first insertion to the control array
                    }
                    else
                    {
                        foreach($this->fileNames as $exisFile)      //< check for same file name
                            if ($exisFile == $fileName[1]) {
                                Errors::putError(Errors::$ArgError, 12);
                            }

                        array_push($this->fileNames, $fileName[1]);
                    }
                }
                if ($argv[$i] == "--loc") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats
                    
                    Stats::writeLoc($this->actFile, $insCounter, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--comments") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeComents($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--jumps") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeJumps($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--labels") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeLabels($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--fwjumps") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeFw_Jumps($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--bwjumps") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeBw_Jumps($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
                if ($argv[$i] == "--badjumps") {
                    if (!$this->statSet)
                        Errors::putError(Errors::$ArgError, 10);    //< missing --stats

                    Stats::writeBad_jumps($this->actFile, $this->firstWrite);
                    if ($this->firstWrite)
                    {    
                        $this->firstWrite = FALSE;
                    }
                }
            }
        }
    }
    /**
     * @brief Prints hint for user
     */
    public function printHelp() {

        fwrite(STDOUT, "Program parser.php parses input code file (IPPcode2021), checks it's lexical and syntactic correctness and transforms it to output xml format\n");
        fwrite(STDOUT, "--usage of arguments: \n");
        fwrite(STDOUT, "--help: prints this hint\n");
        fwrite(STDOUT, "--stats='file': indicates that stats of input code will be written to 'file'\n");
        fwrite(STDOUT, "--loc: number of instructions in code\n");
        fwrite(STDOUT, "--comments: number of comments in code\n");
        fwrite(STDOUT, "--labels: number of unique labels in code\n");
        fwrite(STDOUT, "--jumps: number of jumps in code (CALL, JUMP, JUMPIFEQ, JUMPIFNEQ, RETURN\n");
        fwrite(STDOUT, "--fwjumps: number of forward jumps in code\n");
        fwrite(STDOUT, "--bwjumps: number of backward jumps in code\n");
        fwrite(STDOUT, "--badjumps: number of jumps to not existing labels in code\n");
    }
 }

 /** 
  * @class Stats
  * @brief Provides interface for storing stats info and writing them to out files
  */
 class Stats {

    public static $comments = 0;
    public static $labels = 0;
    public static $jumps = 0;
    public static $fw_jumps = 0;
    public static $bw_jumps = 0;
    public static $bad_jumps = 0;
    public static $labelsArray = [];            //< stores all labels read so far
    public static $bad_jumpsArray = [];         //< stores all potentional bad jumps

    /**
     * @brief Writes num of instructions to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeLoc($fileName, $insCounter, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, $insCounter."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, $insCounter."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }  
    }
    /**
     * @brief Writes num of comments to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeComents($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$comments."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$comments."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Writes num of labels to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeLabels($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$labels."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$labels."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Writes num of jumps to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeJumps($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$jumps."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$jumps."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Writes num of forward jumps to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeFw_Jumps($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$fw_jumps."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$fw_jumps."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Writes num of backward jumps to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeBw_Jumps($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$bw_jumps."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$bw_jumps."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Writes num of bad jumps to out file
     * @param $firstWrite Bool flag, if is set we overwrite file, else append to existing file
     */
    public static function writeBad_jumps($fileName, $firstWrite) {

        if ($firstWrite) {
            if (!file_put_contents($fileName, Stats::$bad_jumps."\n")) {
            
                Errors::putError(Errors::$outfileError, 12);
            }
        }
        else if (!file_put_contents($fileName, Stats::$bad_jumps."\n", FILE_APPEND)) {
            
            Errors::putError(Errors::$outfileError, 12);
        }
    }
    /**
     * @brief Determines type of jump
     * @param $actJump Actual type of jump that was read from input file
     */
    public static function check_jumpType($actJump) {

        if (!in_array($actJump, Stats::$labelsArray))   //< unknown label => potentional bad_jump or fw_jump
        {
            Stats::$bad_jumps++;
            Stats::$fw_jumps++;
            array_push(Stats::$bad_jumpsArray, $actJump);
        }
        else {
            Stats::$bw_jumps++;
        }
    }
    /**
     * @brief Edits stats of bad jumps
     */
    public static function edit_badJumps() {
    
        foreach(Stats::$bad_jumpsArray as $badJump)            
        {
            if (!in_array($badJump, Stats::$labelsArray)) {     //< potentional bad label was not found in labels read from whole file 
                Stats::$fw_jumps--;                             //< it was really a bad jump
            }
            else {
                Stats::$bad_jumps--;                            // it was forrward jump
            }
        }       
    } 
 }
/**
 * @class WriteXML
 * @brief Provides interface for writing the XML format of instructions
 */
 class WriteXML {

    private $xml;
    private $argOrder;

    function __construct() {

        $this->xml = xmlwriter_open_memory(); 
        xmlwriter_set_Indent($this->xml ,true);
        xmlwriter_start_Document($this->xml,'1.0', 'UTF-8');
        xmlwriter_start_element($this->xml, 'program');
        xmlwriter_write_attribute($this->xml, 'language', 'IPPcode21');
    }
     /**
     * @brief Writes instruction in XML format
     * @param $insName Name of actual instruction (operation code)
     * @param $insArg Array of arguments of instruction in format [type of arg => value of arg]
     * @param $insCounter Number of actual instruction
     */
    public function writeInstruction($insName, $insArg, $insCounter) {

        $this->argOrder = 1;
        xmlwriter_start_element($this->xml, 'instruction');
        xmlwriter_write_attribute($this->xml, 'order', $insCounter);
        xmlwriter_write_attribute($this->xml, 'opcode', $insName);

        foreach($insArg as $args)
            foreach($args as $type => $value)
            {
                xmlwriter_start_element($this->xml, 'arg'.$this->argOrder);
                xmlwriter_write_attribute($this->xml, 'type', $type);
                
                if ($type == 'bool')
                    $value = strtolower($value);
              
                xmlwriter_text($this->xml, $value);
                xmlwriter_end_element($this->xml);
                $this->argOrder++;
            }
    }

    public function endElemenet(){

        xmlwriter_end_element($this->xml);
    }

    public function flushXML() {

        xmlwriter_end_document($this->xml);
        echo xmlwriter_output_memory($this->xml);
    }
 }
 /**
 * @class Parser
 * @brief Provides interface for checking lexical and syntax correctness
 */
 class Parser {

    public $insName;
    public $insArg;
    public $insCounter = 0;
    private $headRead = FALSE;

    /**
     * @brief Inits instruction name and array of argumets after each line is read 
     */
    public function initInsVars() {

        unset($this->insName);
        $this->insArg = [];
    }
    /**
     * @brief Reads line from file, removes whitespaces, commnets, blank lines
     * @return False when all file is read, true in case we want next line 
     */
    public function getInstruction() {

        $this->initInsVars();
        if ($line = fgets(GlobalStuff::$stdin))
        {    
            if ($line == "\n") 
            {
                return TRUE;
            }

            $this->insCounter++;
            $line = preg_replace('/\s+/', ' ', $line);   //< remove multiple whitespaces
            $line = $this->removeComments($line);

            if ($line == "")        //< the whole line is a comment
            {
                return TRUE;
            }

            $line = trim($line);
            $insItems = explode(' ', $line);
            if ($this->insCounter == 1 && $this->headRead == FALSE)     // header was not read yet
            {
                $insItems[0] = strtoupper($insItems[0]);
                if (strcmp($insItems[0], ".IPPCODE21") == 0 && count($insItems) == 1)
                {
                    $this->insCounter--;    //< header is not instruction
                    $this->headRead = TRUE;
                    return TRUE;
                }
                else
                {
                    Errors::putError(Errors::$headError, 21);
                }
            }

            if ($this->checkInstruction($insItems))
            {
                return TRUE;        //< get another instruction
            }
            else
            {
                Errors::putError(Errors::$syntaxError, 23);
            }
        }
        else if ($line == FALSE && $this->insCounter == 0 && $this->headRead == FALSE)  //< empty file
        {
            Errors::putError(Errors::$headError, 21);
        }
        else        //< there is nothing to read
        {
            return FALSE;
        }
    }
    /**
     * @brief Removes comments form file
     * @return Instruction before # or "" in case the whole file was a comment
     */
    public function removeComments($line) {

        $line = explode('#', $line);
        if (count($line) > 1) {
            Stats::$comments++;
            if ($line[0] == "")     //< whole line was a comment
            {
                $this->insCounter--;
            }
        }
        return $line[0];
    }
    /**
     * @brief Checks lexical and syntax correctness of instruction
     * @param $insItems Whole line parsed by ' '
     * @return False when erros occures, otherwise True 
     */
    public function checkInstruction($insItems) {

        // depends on type of opcode how many and which arguments we require
        switch ($insItems[0] = strtoupper($insItems[0])) {
            case 'MOVE':
                if (count($insItems) != 3)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1]) && $this->checkSymbol($insItems[2])); 
                }
                break;
            case 'CREATEFRAME':
            case 'PUSHFRAME':
            case 'POPFRAME':
            case 'BREAK':
                if (count($insItems) != 1)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return TRUE;
                }
                break;
            case 'RETURN':
                if (count($insItems) != 1)
                    return FALSE;
                else
                {
                    Stats::$jumps++;
                    $this->insName = $insItems[0];
                    return TRUE;
                }
                break;
            case 'DEFVAR':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1])); 
                }
                break;
            case 'LABEL':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    if (!in_array($insItems[1], Stats::$labelsArray)) {
                        Stats::$labels++;       //< counts only original labels
                    }
                    array_push(Stats::$labelsArray, $insItems[1]);
                    $this->insName = $insItems[0];
                    return ($this->checkLabel($insItems[1]));
                }
                break;
            case 'PUSHS':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkSymbol($insItems[1]));
                }
                break;
            case 'POPS':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1]));
                }
                break;
            case 'ADD':
            case 'SUB':
            case 'MUL':
            case 'IDIV':
            case 'LT':
            case 'GT':
            case 'EQ':
            case 'AND':
            case 'OR':
            case 'STRI2INT':
            case 'CONCAT':
            case 'GETCHAR':
            case 'SETCHAR':
            case 'OR':
            case 'STRI2INT':
                if (count($insItems) != 4)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1]) && $this->checkSymbol($insItems[2]) && $this->checkSymbol($insItems[3]));
                }
                break;
            case 'NOT':
            case 'INT2CHAR':
            case 'STRLEN':
            case 'TYPE':
                if (count($insItems) != 3)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1]) && $this->checkSymbol($insItems[2]));
                }
                break;
            case 'READ':
                if (count($insItems) != 3)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkVar($insItems[1]) && $this->checkType($insItems[2]));
                }
                break;
            case 'WRITE':
            case 'EXIT':
            case 'DPRINT':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    $this->insName = $insItems[0];
                    return ($this->checkSymbol($insItems[1]));
                }
                break;
            case 'JUMP':
            case 'CALL':
                if (count($insItems) != 2)
                    return FALSE;
                else
                {
                    Stats::$jumps++;
                    Stats::check_jumpType($insItems[1]);
                    $this->insName = $insItems[0];
                    return ($this->checkLabel($insItems[1]));
                }
                break;
            case 'JUMPIFEQ':
            case 'JUMPIFNEQ':
                if (count($insItems) != 4)
                    return FALSE;
                else
                {
                    Stats::$jumps++;
                    Stats::check_jumpType($insItems[1]);
                    $this->insName = $insItems[0];
                    return ($this->checkLabel($insItems[1]) && $this->checkSymbol($insItems[2]) && $this->checkSymbol($insItems[3]));
                }
            default:
                return FALSE;
                break;
        }
    }
    /**
     * @brief Checks lexical and sytactic correctness of variable
     * @param $var Current variable which is checked
     * @return False when incorrect, True otherwise
     */
    public function checkVar($var) {

        if (preg_match('/^(GF|LF|TF)@([A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])([0-9]|[A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])*$/', $var))
        {
            array_push($this->insArg, ['var' => $var]);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    /**
     * @brief Checks lexical and sytactic correctness of symbol
     * @param $symbol Current symbol which is checked
     * @return False when incorrect, True otherwise
     */
    public function checkSymbol($symbol) {

        if (preg_match('/^(GF|LF|TF)@.*$/', $symbol))    //< symbol is variable
        {
            return $this->checkVar($symbol);
        }
        else if (preg_match('/^(int|bool|string|nil)@.*$/', $symbol))    //<symbol is constant
        {
            $symbol = explode('@', $symbol, 2);
            switch ($symbol[0]) {
                case 'int':
                    if ($symbol[1] != "")       //< there must be value of symbol
                    {
                        array_push($this->insArg, ['int' => $symbol[1]]);
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                    break;

                case 'bool':
                    if (preg_match('/^(true|false)$/', $symbol[1]))
                    {
                        array_push($this->insArg, ['bool' => $symbol[1]]);
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                    break;

                case 'string':
                    if (preg_match('/^(\\\\[0-9]{3}|[^[:blank:]\\\\])*$/', $symbol[1]))
                    {
                        array_push($this->insArg, ['string' => $symbol[1]]);
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                    break;

                case 'nil':
                    if ($symbol[1] != "")       //< there must be value of symbol
                    {
                        array_push($this->insArg, ['nil' => $symbol[1]]);
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                    break;
                
                default:
                    return FALSE;
                    break;
            }
        }
    }
    /**
     * @brief Checks lexical and sytactic correctness of label
     * @param $label Current label which is checked
     * @return False when incorrect, True otherwise
     */
    public function checkLabel($label) {

        if (preg_match('/^([A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])([0-9]|[A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])*$/', $label))
        {
            array_push($this->insArg, ['label' => $label]);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    /**
     * @brief Checks sytactic correctness of type (instruction read)
     * @param $type Current type which is checked
     * @return False when incorrect, True otherwise
     */
    public function checkType($type) {

        if (preg_match('/^(int|string|bool)$/', $type))
        {
            array_push($this->insArg, ['type' => $type]);
            return TRUE;
        }
        else
        {
        return FALSE;
        }
    }
 }

 #main
 GlobalStuff::openFile();
 $xmlWriter = new WriteXML();
 $parser = new Parser();
 $argParser = new ArgumentParser();

 $argParser->checkHelp();       //< check help argument in case there is no input file, we read arguments after input file is read
                                //< so we can write hint for user immediately

 while($parser->getInstruction())
 {
     if (isset($parser->insName))       //< writes only instructions
     {
         $xmlWriter->writeInstruction($parser->insName, $parser->insArg, $parser->insCounter);
         $xmlWriter->endElemenet();
     }
 }

 Stats::edit_badJumps();                            //< count of bad jumps is edited when whole file is read
 $argParser->getArguments($parser->insCounter);     //< check arguments after whole file is read, cause stats are ready to write
 $xmlWriter->endElemenet();
 $xmlWriter->flushXML();
 
 fclose(GlobalStuff::$stdin);
 exit(0);

?>