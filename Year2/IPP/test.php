<?php

/**
 * @file test.php
 * @brief Provides automatic tests for parse.php and interpret.py  
 * @author Marek Miček, xmicek08
 * @date 29.3.2021
 */

ini_set('display_errors', 'stderr');

#main
$argParser = new ArgumentParser();
$argParser->getArguments();
$argParser->check_input_Files();

$dirIterator = new DirectoryScanner();
$dirIterator->scanDirectory($argParser);


class ArgumentParser
{
    public $directory;
    public $recursive;
    public $parse_script;
    public $int_script;
    public $parse_only;
    public $int_only;
    public $jexamxml;
    public $jexamcfg;

    public function __construct()
    {
        $this->directory = getcwd().'/';
        $this->recursive = FALSE;
        $this->parse_script = './parse.php';
        $this->int_script = './interpret.py';
        $this->parse_only = FALSE;
        $this->int_only = FALSE;
        $this->jexamxml = '/pub/courses/ipp/jexamxml/jexamxml.jar';
        $this->jexamcfg = '/pub/courses/ipp/jexamxml/options';
    }

    /**
     * @brief Parses arguments of commnad line
     */
    public function getArguments()
    {
        global $argc;
        $validArgs = 0;
        if ($argc > 1)
        {
            $opts = getopt("", ["help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only", "jexamxml:", "jexamcfg:"]);
            if (array_key_exists("help", $opts))
            {
                if ($argc == 2) {
                    $this->printHelp();
                    exit(0);
                }
                else {
                    fwrite(STDERR, "Help parameter can not be combinated with any other parameter !!!\n");
                    exit(10);
                }
            }
            if (array_key_exists("directory", $opts))
            {
                //directory path must end with '/'
                if (substr($opts["directory"], -1) != "/") {
                    $opts["directory"] = $opts["directory"]."/";
                }
                $this->directory = $opts["directory"];
                $validArgs++;
            }
            if (array_key_exists("recursive", $opts))
            {
                $this->recursive = TRUE;
                $validArgs++;
            }
            if (array_key_exists("parse-script", $opts))
            {
                $this->parse_script = $opts["parse-script"];
                $validArgs++;
            }
            if (array_key_exists("int-script", $opts))
            {
                $this->int_script = $opts["int-script"];
                $validArgs++;
            }
            if (array_key_exists("parse-only", $opts))
            {
                // can not be combinated with int-only, int-script
                if ($this->int_only == FALSE && !(array_key_exists("int-script", $opts))) {
                    $this->parse_only = TRUE;
                    $validArgs++;
                }
                else {
                    fwrite(STDERR, "Invalid combination of parameters !!!\n");
                    exit(10);
                }
            }
            if (array_key_exists("int-only", $opts))
            {
                 // can not be combinated with int-only, int-script
                 if ($this->parse_only == FALSE && !(array_key_exists("parse-script", $opts))) {
                    $this->int_only = TRUE;
                    $validArgs++;
                }
                else {
                    fwrite(STDERR, "Invalid combination of parameters !!!\n");
                    exit(10);
                }
            }
            if (array_key_exists("jexamxml", $opts))
            {
                $this->jexamxml = $opts["jexamxml"];
                $validArgs++;
            }
            if (array_key_exists("jexamcfg", $opts))
            {
                $this->jexamcfg = $opts["jexamcfg"];
                $validArgs++;
            }
            // check if there was not invalid parameter entered
            if ($validArgs != $argc - 1) {
                fwrite(STDERR, "Unknown parameter entered !!!\n");
                fwrite(STDERR, "Run 'php7.4 test.php --help' to see valid parameters \n");
                exit(10);
            }
        }
    }

    /**
     * @brief Checks existence of input files
     */
    public function check_input_Files()
    {
        if (!file_exists($this->directory) || !file_exists($this->parse_script) || !file_exists($this->int_script) || !file_exists($this->jexamxml) || !file_exists($this->jexamcfg))
        {
            fwrite(STDERR, "Input file does not exists !!!\n");
            exit(41);
        }
    }

    /**
     * @brief Prints hint for user
     */
    public function printHelp()
    {
        fwrite(STDOUT, "Program test.php searches in current or specific directory for tests which check programs parse.php and interpret.py\n");
        fwrite(STDOUT, "It prints to STDOUT html representation of results of tests\n");
        fwrite(STDOUT, "--usage of arguments: \n");
        fwrite(STDOUT, "--help: prints this hint\n");
        fwrite(STDOUT, "--directory: search for test in entered directory, if missing then in current working directory\n");
        fwrite(STDOUT, "--recursive: serach recursively in all subfolders of entered directory\n");
        fwrite(STDOUT, "--parse-script: file for analysis of input IPPcode21 code\n");
        fwrite(STDOUT, "--int-cript: file for interpretation of XML representation of IPPcode21\n");
        fwrite(STDOUT, "--parse-only: only file for analysis of IPPcode21 will be tested, can not be combinated with --int-script and int-only\n");
        fwrite(STDOUT, "--int-only: only file for XML interpretation will be tested, can not be combinated with --parse-script and parse-only\n");
        fwrite(STDOUT, "--jexamxml: file with A7Soft JExamXML\n");
        fwrite(STDOUT, "--jexamcfg: file with configuration of A7Soft JExamXML\n");
    }
}

class DirectoryScanner
{
    public $script_tester;

    public function __construct()
    {
        $this->script_tester = new ScriptTester();
    }

    /**
     * @brief Searches files for testing in entered directory
     */
    public function scanDirectory($argParser)
    {
        $directoryIterator = new RecursiveDirectoryIterator($argParser->directory);
        if ($argParser->recursive == TRUE) {
            $fileIterator = new RecursiveIteratorIterator($directoryIterator);
        }
        else {
            $fileIterator = new IteratorIterator($directoryIterator);
        }

        // we serach only for .src files
        $fileRegex = new RegexIterator($fileIterator, '/^.+\.src$/i', RecursiveRegexIterator::GET_MATCH);
        foreach($fileRegex as $file)
        {
            // init new item in directory array
            $dir_path = $this->script_tester->get_dir_path($file[0]);
            $this->script_tester->init_dir_array($dir_path);

            $this->in_out_rec_existence(substr($file[0], 0, -4));
            $this->script_tester->test_parse_script($file[0], $argParser);
        }
    }

    /**
     * @brief Checks existence of .in/.out/.rc files, generates them if not exist
     */
    public function in_out_rec_existence($file_name)
    {
        
        if (!file_exists($file_name.".in"))
        {
            if (file_put_contents($file_name.".in", "") === FALSE) {
                fwrite(STDERR, "Unable to make ".$file_name.".in file !!!\n");
                exit(12);
            }
        }
        if (!file_exists($file_name.".out"))
        {
            if (file_put_contents($file_name.".out", "") === FALSE) {
                fwrite(STDERR, "Unable to make ".$file_name.".out file !!!\n");
                exit(12);
            }
        }
        if (!file_exists($file_name.".rc"))
        {
            if (file_put_contents($file_name.".rc", "0") === FALSE) {
                fwrite(STDERR, "Unable to make ".$file_name.".rc file !!!\n");
                exit(12);
            }
        }
    }
}


class ScriptTester
{
    public $dir_path;
    public $test_name;
    public $directories = [];
    public $dir_stats = array();
    public $results = array();
    
    /**
     * @brief Inits new item in dir array everytime new dir with .src file is found in folder
     */
    public function init_dir_array($dir_path)
    {
        if (!in_array($dir_path, $this->directories)) //< store directory only once
        {
            $this->directories[] = $dir_path;
            $this->dir_path = $dir_path;
            $this->dir_stats[$dir_path]["found"] = 0;
            $this->dir_stats[$dir_path]["passed"] = 0;
        }
    }

    /**
     * @brief Returns path to directory where is current test
     */
    public function get_dir_path($file_path)
    {
       return preg_replace('/^(.*\/).+\.src$/', '\1', $file_path);
    }

    /**
     * @brief Returns name of current test
     */
    public function get_file_name($file_path)
    {
       return preg_replace('/^(.*\/)?(.+)\.src$/', '\2', $file_path);   
    }

    /**
     * @brief Executes parse script and tests it's output
     */
    public function test_parse_script($file_path, $argParser)
    {
        //get file name and dir path from path
        $file_name = $this->get_file_name($file_path);
        $dir_path = $this->get_dir_path($file_path);

        $this->results[$dir_path][$file_name]["test"] = $file_name;
        $this->results[$dir_path][$file_name]["expected"] = "pass";
        $this->results[$dir_path][$file_name]["real"] = "pass";
        $this->results[$dir_path][$file_name]["out"] = "OK";
        $this->results[$dir_path][$file_name]["rc"] = "OK";
        $this->dir_stats[$this->dir_path]["found"]++;
        $this->dir_stats[$this->dir_path]["passed"]++;
        $this->test_name = $file_name;

        if (file_get_contents($dir_path.$file_name.".rc") != 0) {
            $this->results[$dir_path][$file_name]["expected"] = "fail";
        }
        
        if ($argParser->int_only == FALSE) 
        {
            exec("php7.4 ".$argParser->parse_script." < ".$dir_path.$file_name.".src"." > tmp_".$file_name.".out", $dump, $return_value);
            if ($return_value != 0)
            {
                if (file_get_contents($dir_path.$file_name.".rc") != $return_value) {
                    $this->results[$this->dir_path][$this->test_name]["rc"] = "NOK";
                    $this->dir_stats[$this->dir_path]["passed"]--;
                }
                // can not iterpret when parse.php did not return 0
                $this->results[$this->dir_path][$this->test_name]["real"] = "fail";
                unlink("tmp_".$file_name.".out");
                return;
            }
            
            // work with A7Soft only if parse_only was entered by user
            if ($argParser->parse_only == TRUE) {
                exec("java -jar ".$argParser->jexamxml." tmp_".$file_name.".out ".$dir_path.$file_name.".out"." diffs.xml ".$argParser->jexamcfg, $dump, $return_value);
                if ($return_value != 0) {
                    $this->results[$this->dir_path][$this->test_name]["out"] = "NOK";
                    $this->dir_stats[$this->dir_path]["passed"]--;
                }
            }

            // just test interpret, .out file contains output of interpret not parser
            else {
                $this->test_int_script($dir_path, $file_name, $argParser);
            }
        }
        // interpret only
        else {
            $this->test_int_script($dir_path, $file_name, $argParser);
        }

        $this->unlink_file($file_name, $argParser);
    }

    /**
     * @brief Executes int script and tests it's output
     */
    public function test_int_script($dir_path, $file_name, $argParser)
    {
        // location of xml file differs when testing parse and interpret together
        if ($argParser->int_only == FALSE) {
            exec("python3.8 ".$argParser->int_script." --source=tmp_".$file_name.".out --input=".$dir_path.$file_name.".in > "."tmp2_".$file_name.".out", $dump, $return_value);
        }
        else {
            exec("python3.8 ".$argParser->int_script." --source=".$dir_path.$file_name.".src --input=".$dir_path.$file_name.".in > "."tmp2_".$file_name.".out", $dump, $return_value);
        }

        if ($return_value != 0)
        {
            if (file_get_contents($dir_path.$file_name.".rc") != $return_value) {        
                $this->results[$this->dir_path][$this->test_name]["rc"] = "NOK";
                $this->dir_stats[$this->dir_path]["passed"]--;
            }
            // not make diff if interpret returned non zero value
            $this->results[$this->dir_path][$this->test_name]["real"] = "fail"; 
            return;
        }

        exec("diff tmp2_".$file_name.".out ".$dir_path.$file_name.".out", $dump, $return_value);
        if ($return_value != 0)
        {
            $this->results[$this->dir_path][$this->test_name]["out"] = "NOK";
            $this->dir_stats[$this->dir_path]["passed"]--;
        }
    }

    /**
     * @brief Unlinks temporary files on the basis of user's parameters
     */
    public function unlink_file($file_name, $argParser)
    {
        if ($argParser->int_only == TRUE) {
            unlink("tmp2_".$file_name.".out");
        }
        else if ($argParser->parse_only == TRUE) {
            unlink("tmp_".$file_name.".out");
            unlink("diffs.xml");    
        }
        else {
            unlink("tmp_".$file_name.".out");
            unlink("tmp2_".$file_name.".out");
        }
    }
}
?>

<!DOCTYPE HTML>
<html lang="cs">
    <head>
        <meta charset="utf-8"/>
        <meta name="IPP 2020/2021 test.php output" />

        <style>
        body {
            background-color: #BDB76B;
            margin: 35px;
        }    
        h1 {
            font-size: 30px;
            font-family: Arial, Helvetica, sans-serif;
        }
        table, th, td {
            background-color: #fff;
            border: 2px solid black;
            text-align: center;

        }
        .container {
            margin: auto;
            width: 50%;
        }
        .header {
            padding: 30px;
            text-align: center;
            background-color: #A9A9A9;
        }
        .table_head {
            margin-top: 15px;
        }   
        </style>

        <title>IPP test results of script test.php</title>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h1>IPP test results of script test.php</h1>
            </div>

<?php
    $dir_stats = $dirIterator->script_tester->dir_stats;
    $result = $dirIterator->script_tester->results;   
    foreach ($dirIterator->script_tester->directories as $dir)
    {
?>            
            <div class="tables">
                <h3 class="table_head">Directory: "<?php echo $dir; ?>" Found: "<?php echo $dir_stats[$dir]["found"]; ?>" Passed: "<?php echo $dir_stats[$dir]["passed"] ?>"</h3>
                <table>
                    <thead>
                        <tr>
                            <th>Test name</th>
                            <th>Expected</th>
                            <th>Real</th>
                            <th>Out</th>
                            <th>Rc</th>
                        </tr>
                    </thead>
                    <tbody>
<?php
        foreach ($result[$dir] as $row)
        {
?>
                        <tr>
                            <td><?php echo $row["test"] ?></td>
                            <td><?php echo $row["expected"] ?></td>
                            <td><?php echo $row["real"] ?></td>
                            <td><?php echo $row["out"] ?></td>
                            <td><?php echo $row["rc"] ?></td>
                        </tr>
    <?php } ?>
                    </tbody>
                </table>
            </div>
<?php  } ?> 

        </div>
    </body>
</html>