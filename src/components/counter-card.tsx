"use client";

import { useEffect, useState } from "react";
import { ref, onValue, set, serverTimestamp } from "firebase/database";
import { db } from "@/config/firebase";
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { useToast } from "@/hooks/use-toast";
import { AlertTriangle, Loader2, Package, RotateCcw } from "lucide-react";
import { Skeleton } from "./ui/skeleton";

const COUNTER_PATH = "products/count";
const RESET_COMMAND_PATH = "commands/reset";
const COUNTER_LIMIT = 100;

export default function CounterCard() {
  const [count, setCount] = useState<number | null>(null);
  const [loading, setLoading] = useState(true);
  const [isResetting, setIsResetting] = useState(false);
  const { toast } = useToast();

  useEffect(() => {
    const counterRef = ref(db, COUNTER_PATH);
    const unsubscribe = onValue(counterRef, (snapshot) => {
      const data = snapshot.val();
      setCount(data);
      setLoading(false);

      if (data !== null && data >= COUNTER_LIMIT) {
        toast({
          title: "Alert: Limit Reached",
          description: `The product count has reached the limit of ${COUNTER_LIMIT}.`,
          variant: "destructive",
          duration: 5000,
        });
      }
    }, (error) => {
        console.error("Firebase read failed: ", error);
        setLoading(false);
        toast({
            title: "Connection Error",
            description: "Could not fetch data from the database.",
            variant: "destructive",
        });
    });

    return () => unsubscribe();
  }, [toast]);

  const handleReset = async () => {
    setIsResetting(true);
    try {
      const resetRef = ref(db, RESET_COMMAND_PATH);
      await set(resetRef, serverTimestamp());
      toast({
        title: "Command Sent",
        description: "Reset signal sent to the device.",
      });
    } catch (error) {
      console.error("Failed to send reset command: ", error);
      toast({
        title: "Error",
        description: "Failed to send reset command.",
        variant: "destructive",
      });
    } finally {
      setIsResetting(false);
    }
  };

  const countExceeded = count !== null && count >= COUNTER_LIMIT;

  return (
    <Card className="w-full max-w-md shadow-lg">
      <CardHeader>
        <div className="flex items-center gap-3">
            <Package className="w-6 h-6 text-primary" />
            <CardTitle className="font-headline text-2xl">Product Counter</CardTitle>
        </div>
        <CardDescription>Real-time count from IoT device.</CardDescription>
      </CardHeader>
      <CardContent className="flex flex-col items-center justify-center py-12">
        {loading ? (
            <Skeleton className="h-28 w-48" />
        ) : (
        <div 
          className={`font-headline font-bold text-8xl md:text-9xl transition-colors ${
            countExceeded ? 'text-destructive' : 'text-primary'
          }`}
        >
          {count !== null ? count : "--"}
        </div>
        )}
        <div className="flex items-center gap-2 mt-4 text-muted-foreground">
            {countExceeded && <AlertTriangle className="w-4 h-4 text-destructive"/>}
            <p className={`${countExceeded ? 'text-destructive font-medium' : ''}`}>
                Limit: {COUNTER_LIMIT}
            </p>
        </div>
      </CardContent>
      <CardFooter>
        <Button
          onClick={handleReset}
          className="w-full"
          variant="outline"
          disabled={isResetting || loading}
        >
          {isResetting ? (
            <Loader2 className="mr-2 h-4 w-4 animate-spin" />
          ) : (
            <RotateCcw className="mr-2 h-4 w-4" />
          )}
          Reset Counter
        </Button>
      </CardFooter>
    </Card>
  );
}
