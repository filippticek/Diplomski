import React, {useEffect, useState} from "react";
import './App.css'
import Nav from "./components/Nav";
import Login from "./components/Login";
import Register from "./components/Register";
import Dashboard from "./components/Dashboard";
import Device from "./components/Device";
import {BrowserRouter as Router, Switch, Route} from 'react-router-dom';


function App(){
  
  useEffect(() => {
    fetchLogin();
  }, []);

  const [logedIn, setLogedIn] = useState(false);

  const fetchLogin = async () => {
    const logedIn = true;
    setLogedIn(logedIn);
  }

  return (
    <Router>
      <div className="App">
        <Nav />
        <Route path="/dashboard" component={Dashboard}/>
        <Route path="/login" component={Login}/>
        <Route path="/register" component={Register}/>
      </div>
    </Router>  
  );
}

export default App;