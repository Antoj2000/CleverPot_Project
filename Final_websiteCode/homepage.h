
String homePagePart1 = F(R"=====(<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
  <meta charset="utf-8">
  <meta http-equiv="refresh" content="5"> <!-- Refresh the page every 5 seconds -->
  <!-- Make page respond to screen size !-->
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
 
<!-- Title --> s
  <title> CleverPot WebServer </title>
    <style>
      
      body {       
        background-color: #f5f5f5;
        font-family: Arial, sans-serif;
        margin: 0;
        padding: 0;
      }
      header {
        background-color: #339966; <-- Green header -->
        colour: white;
        text-align: center;
        padding: 20px 0;
      }
      .Container{
          max-width: 800px;
          margin: 20px auto;
          padding: 20px;
          background-color: white;
          border-radius: 10px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
          }

      table{
        width: 100%;
          border-collapse: collapse;
          margin-top: 20px;
      }
      
      th, td {
        padding: 8px;
        text-align: left;
        border-bottom: 1px solid #ddd;
    }

    </style>
</head>
	<body>
		<header>
     <h1> CleverPot </h1>
     <p>Welcome to the CleverPot Homepage</p>
    </header>

  <div class="container">
		<table>
        <tr>
          <th>Sensor</th>
          <th>Value</th>
          <th>Unit</th>
        </tr>
        <tr>
          <td>Temperature</td>
          <td> )=====");
String homePagePart2 = F(R"=====(</td>  <!--Temperature reading -->
         <td>Degree Cel</td>
        </tr>
        <tr>
          <td>Humidity</td>
          <td>)=====");
String homePagePart3 = F(R"=====(</td>  <!--humidity reading -->
          <td>g/kg</td>
        </tr>
        <tr>
          <td>Water Level</td>
          <td>)=====");
String homePagePart4 = F(R"=====(</td>   <!--water level reading -->
          <td> % full</td>
          </tr>
        <tr>
          <td>Soil Moisture</td>
          <td>)=====");
String homePagePart5 = F(R"=====(</td>   <!--soil mositure reading -->
          <td> %</td>
      </table>
      <form action="/activatePump" method="post">
        <button type="submit"style="background-color: #4CAF50; /* Green */
                                border: none;
                                color: white;
                                padding: 15px 32px;
                                text-align: center;
                                text-decoration: none;
                                display: inline-block;
                                font-size: 16px;
                                margin: 4px 2px;
                                cursor: pointer;
                                border-radius: 8px;">Activate Pump</button
      </form>
	</body>
</html>)=====");
