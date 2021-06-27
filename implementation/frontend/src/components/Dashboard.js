import React from "react";
import '../App.css';
import {Line} from 'react-chartjs-2';

function Dashboard(){
  return (
    <div>
        <Line 
            data={{
                labels: ['One', 'Two'],
                datasets: [
                    { label:"label",
                      data:[1,2],
                    },
                ],
            }}
            height={200}
            width={400}
            options={{
                maintainAspectRatio: false,
            }}
        />
    </div>
  );
}

export default Dashboard