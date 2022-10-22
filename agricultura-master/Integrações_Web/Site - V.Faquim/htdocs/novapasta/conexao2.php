<?php
    if (isset($_GET['inf'])) {
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "aula";
        $conn = mysqli_connect($servername, $username, $password, $dbname);
        $inf = $_GET["inf"];
        $sql="INSERT INTO arduino (inf) VALUES ('$inf')";
        if (!$conn) {
            die("Connection failed: " . mysqli_connect_error());
        }

        if ($conn->query($sql) === TRUE) {
            echo "Cadastro salvo com sucesso";
        } else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }

        $conn->close();
    }
    ?>